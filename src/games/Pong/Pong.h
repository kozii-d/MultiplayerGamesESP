#ifndef PONG_H
#define PONG_H

#define LEFT_BUTTON_PIN D1
#define RIGHT_BUTTON_PIN D2
#define CENTER_BUTTON_PIN D3

#include <EncButton.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <TimerMs.h>

#include "Puddle.h"
#include "Ball.h"
#include "Score.h"
#include "../common/Lobby.h"
#include "../../utils/Menu.h"

class Pong {
public:
    Pong(DisplayManager &oled) : 
    oled(oled), wsServer(80), 
    leftButton(LEFT_BUTTON_PIN), rightButton(RIGHT_BUTTON_PIN),
    leftPuddle(oled, 0, SCREEN_HEIGHT / 2 - PUDDLE_HEIGHT / 2, PUDDLE_WIDTH, PUDDLE_HEIGHT, 1),
    rightPuddle(oled, SCREEN_WIDTH - PUDDLE_WIDTH, SCREEN_HEIGHT / 2 - PUDDLE_HEIGHT / 2, PUDDLE_WIDTH, PUDDLE_HEIGHT, 1),
    ball(oled, leftPuddle, rightPuddle),
    score(oled, 0, 0),    
    lobby("ESP_Pong"),
    menu(oled, LEFT_BUTTON_PIN, RIGHT_BUTTON_PIN, CENTER_BUTTON_PIN),
    renderTimer(RENDER_INTERVAL, true),
    buttonTimer(BUTTON_PRESS_INTERVAL, true),
    ballTimer(BALL_MOVE_INTERVAL, true),
    showScoreTimer(SHOW_SCORE_TIME, false, true),
    wsTimer(SEND_DATA_INTERVAL, true)
    {};

    void loop() {
        lobby.loop();

        // fixme
        if (isGameRunning) {
            handleInput();
            handleScoreTimerEnd();
            handleBallMovement();
            handleWebSocket();
            handleRendering();
        } else {
            menu.loop();
        }
    };

    void begin() {
        setupMenu();
    };

private:
    static constexpr uint16_t PUDDLE_HEIGHT = SCREEN_HEIGHT / 4;
    static constexpr uint16_t PUDDLE_WIDTH = 2;

    static constexpr uint16_t BUTTON_PRESS_INTERVAL = 16;
    static constexpr uint16_t RENDER_INTERVAL = 16; // 16 == 60 fps, 33 == 30 fps
    static constexpr uint16_t SEND_DATA_INTERVAL = 16;
    static constexpr uint16_t BALL_MOVE_INTERVAL = 16;
    static constexpr uint16_t ROUND_END_INTERVAL = 2000;
    static constexpr uint16_t SHOW_SCORE_TIME = 2000;

    DisplayManager& oled;
    WebSocketsServer wsServer;
    Button leftButton;
    Button rightButton;
    Puddle leftPuddle;
    Puddle rightPuddle;
    Ball ball;
    Score score;
    Lobby lobby;
    Menu menu;

    TimerMs renderTimer;
    TimerMs buttonTimer;
    TimerMs ballTimer;
    TimerMs showScoreTimer;
    TimerMs wsTimer;

    bool isRoundEnded = false;
    bool isGameRunning = false;
    bool isServer = false;

    void render() {
        oled.clear();

        if (isRoundEnded) {
            score.draw();
        } else {
            leftPuddle.draw();
            rightPuddle.draw();
            ball.draw();
        }

        oled.update();
    };

    void startGame() {
        isGameRunning = true;
        render();
    };
    void stopGame() {
        isGameRunning = false;
    };

    void onLobbyReady() {
        startGame();
    };

    void onCloseConnection() {
        stopGame();
        menu.displayMenu();
    }

    void createLobby() {
        isServer = true;
        oled.showMessage("Waiting for players...");
        lobby.createLobby(
            [this](const uint8_t *data) {
                handleServerEvent(data);
            },
            [this]() {
                onLobbyReady();
            },
            [this]() {
                onCloseConnection();
            }
        );
    }

    void joinLobby() {
        isServer = false;
        oled.showMessage("Connecting to lobby...");
        lobby.joinLobby(
            [this](const uint8_t *data) {
                handleClientEvent(data);
            },
            [this]() {
                Serial.println("Connected");
                onLobbyReady();
            },
            [this]() {
                Serial.println("Disconnected");
                onCloseConnection();
            }
        );
    }

    void checkEndOfRound() {
        if (ball.getX() <= 0) {
            onEndRound(true);
        } else if (ball.getX() >= SCREEN_WIDTH) {
            onEndRound(false);
        }
    };

    void sendGameStateToAllClients() {
        JsonDocument gameStateJsonDoc;
        gameStateJsonDoc["type"] = "UPDATE_GAME_STATE";
        JsonObject gameStatePayloadJsonDoc = gameStateJsonDoc["payload"].to<JsonObject>();

        gameStatePayloadJsonDoc["isRoundEnded"] = isRoundEnded;

        gameStatePayloadJsonDoc["leftPuddle"]["y"] = leftPuddle.getY();
        // DO NOT SEND RIGHT PUDDLE POSITION TO CLIENT. Client proccess it locally

        gameStatePayloadJsonDoc["ball"]["x"] = ball.getX();
        gameStatePayloadJsonDoc["ball"]["y"] = ball.getY();
        
        gameStatePayloadJsonDoc["score"]["left"] = score.getLeft();
        gameStatePayloadJsonDoc["score"]["right"] = score.getRight();

        String gameStateJson;
        serializeJson(gameStateJsonDoc, gameStateJson);

        lobby.sendToAll(gameStateJson);
    }

    void sendResetRightPuddle() {
        JsonDocument resetRightPuddleJsonDoc;
        resetRightPuddleJsonDoc["type"] = "RESET_RIGHT_PUDDLE";
        String resetRightPuddleJson;
        serializeJson(resetRightPuddleJsonDoc, resetRightPuddleJson);
        lobby.sendToAll(resetRightPuddleJson);
    }

    void handleServerEvent(const uint8_t *data) {
        JsonDocument dataJsonDoc;
        deserializeJson(dataJsonDoc, data);
        String type = dataJsonDoc["type"];
        JsonObject payload = dataJsonDoc["payload"].as<JsonObject>();
        if (type == "UPDATE_RIGHT_PUDDLE") {
            rightPuddle.setY(payload["y"].as<int>());
        }
    }

    void handleClientEvent(const uint8_t *data) {
        JsonDocument dataJsonDoc;
        deserializeJson(dataJsonDoc, data);
        String type = dataJsonDoc["type"];
        JsonObject payload = dataJsonDoc["payload"].as<JsonObject>();

        if (type == "RESET_RIGHT_PUDDLE") {
            rightPuddle.setY(SCREEN_HEIGHT / 2 - PUDDLE_HEIGHT / 2);
        }

        if (type == "UPDATE_GAME_STATE") {
            isRoundEnded = payload["isRoundEnded"];
            leftPuddle.setY(payload["leftPuddle"]["y"]);
            // DO NOT SET RIGHT PUDDLE POSITION. Client proccess it locally
            ball.setX(payload["ball"]["x"]);
            ball.setY(payload["ball"]["y"]);
            score.setLeft(payload["score"]["left"]);
            score.setRight(payload["score"]["right"]);
        }
    }

    void movePuddle(PuddleDirection direction) {
        if (isServer) {
            leftPuddle.move(direction);
        } else {
            rightPuddle.move(direction);
            JsonDocument movePuddleJsonDoc;
            movePuddleJsonDoc["type"] = "UPDATE_RIGHT_PUDDLE";
            JsonObject movePuddlePayloadJsonDoc = movePuddleJsonDoc["payload"].to<JsonObject>();
            movePuddlePayloadJsonDoc["y"] = rightPuddle.getY();

            String movePuddleJson;
            serializeJson(movePuddleJsonDoc, movePuddleJson);

            lobby.sendToServer(movePuddleJson);
        }
    }

    void resetPuddles() {
        leftPuddle.setY(SCREEN_HEIGHT / 2 - PUDDLE_HEIGHT / 2);
        rightPuddle.setY(SCREEN_HEIGHT / 2 - PUDDLE_HEIGHT / 2);
        // Client proccess right puddle position locally. 
        // Server must sent event to reset right puddle position on the end of the round
        sendResetRightPuddle();
    }

    void onEndRound(bool isRightSideWon) {
        isRoundEnded = true;
        showScoreTimer.start();
        if (isRightSideWon) {
            score.incrementRight();
        } else {
            score.incrementLeft();
        }
        ball.reset();
        resetPuddles();
    }

    void handleScoreTimerEnd() { 
        if (isRoundEnded && isServer && showScoreTimer.tick()) {
            isRoundEnded = false;
        }
    }

    void handleBallMovement() {
        if (!isRoundEnded && ballTimer.tick()) {
            ball.move();
            checkEndOfRound();
        }
    }

    void handleWebSocket() {
        if (isServer && wsTimer.tick()) {
            sendGameStateToAllClients();
        }
    }

    void handleRendering() {
        if (renderTimer.tick()) {
            render();
        }
    }

    void handleInput() {
        leftButton.tick();
        rightButton.tick();

        if (!(leftButton.pressing() && rightButton.pressing()) && !isRoundEnded) {
            if (leftButton.pressing() && buttonTimer.tick()) {
                movePuddle(PuddleDirection::UP);
            }
            if (rightButton.pressing() && buttonTimer.tick()) {
                movePuddle(PuddleDirection::DOWN);
            }
        }
    };


    void setupMenu() {
        menu.addItem("Create Lobby", [this]() { createLobby(); });
        menu.addItem("Join Lobby", [this]() { joinLobby(); });
        menu.displayMenu();
    };
};

#endif
#ifndef LOBBY_H
#define LOBBY_H

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>

typedef std::function<void(const uint8_t*)> EventHandler;

class Lobby {
public:
    Lobby(const char* name) : 
    name(name), wsServer(80), wsClient() {}

    void createLobby(EventHandler onWsEvent, TrivialCB onWsConnect, TrivialCB onWsDisconnect) {
        createAccessPoint(name);
        wsServer.begin();
        // *data - pointer to array of bytes with data
        wsServer.onEvent([this, onWsEvent, onWsConnect, onWsDisconnect](uint8_t clientId, WStype_t wsEventType, const uint8_t *data, size_t length) {
            if (wsEventType == WStype_TEXT) {
                onWsEvent(data);
            } else if (wsEventType == WStype_CONNECTED) {
                onWsConnect();
                Serial.println("Connected");
                // do connection stuff
            } else if (wsEventType == WStype_DISCONNECTED) {
                onWsDisconnect();
                Serial.println("Disconnected");
                // do disconnection stuff
            }
        }); 
    }

    void joinLobby(EventHandler onWsEvent, TrivialCB onWsConnect, TrivialCB onWsDisconnect) {
        connectToWiFi(name);
        wsClient.begin(WiFi.gatewayIP().toString(), 80, "/");
        wsClient.onEvent([this, onWsEvent, onWsConnect, onWsDisconnect](WStype_t wsEventType, const uint8_t *data, size_t length) {
            if (wsEventType == WStype_TEXT) {
                onWsEvent(data);
            } else if (wsEventType == WStype_CONNECTED) {
                onWsConnect();
                Serial.println("Connected");
                // do connection stuff
            } else if (wsEventType == WStype_DISCONNECTED) {
                onWsDisconnect();
                Serial.println("Disconnected");
                // do disconnection stuff
            }
        });
    }

    void sendToAll(String payload) {
        wsServer.broadcastTXT(payload);
    }

    void sendToServer(String payload) {
        wsClient.sendTXT(payload);
    }

    void loop() {
        wsServer.loop();
        wsClient.loop();
    }

private:
    const char* name;
    WebSocketsServer wsServer;
    WebSocketsClient wsClient;

    void createAccessPoint(const char* name) {
        WiFi.softAP(name);
        Serial.println("Access Point Created!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.softAPIP());
    }

    void connectToWiFi(const char* name) {
        WiFi.begin(name);
        Serial.println("Connecting to network");
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("Connected to network");
        Serial.print("IP Address: ");
        Serial.println(WiFi.gatewayIP().toString());
    }
};

#endif
#ifndef BALL_H
#define BALL_H

class Ball {
public:
    Ball(DisplayManager &oled, Puddle &leftPuddle, Puddle &rightPuddle) :
    oled(oled), leftPuddle(leftPuddle), rightPuddle(rightPuddle) {
        reset();
    };

    void move() {
        x += speedX;
        y += speedY;

        if (checkCollisionWithWall()) {
            speedY = -speedY;
        }

        if (checkCollisionWithPaddle(leftPuddle.getX(), leftPuddle.getY(), leftPuddle.getWidth(), leftPuddle.getHeight())) {
            adjustAngleOnPaddleCollision(leftPuddle.getY(), leftPuddle.getHeight());
            speedX = -speedX;
            increaseSpeed();
        }

        if (checkCollisionWithPaddle(rightPuddle.getX(), rightPuddle.getY(), rightPuddle.getWidth(), rightPuddle.getHeight())) {
            adjustAngleOnPaddleCollision(rightPuddle.getY(), rightPuddle.getHeight());
            speedX = -speedX;
            increaseSpeed();
        }
    

    };

     void reset() {
        x = SCREEN_WIDTH / 2;
        y = SCREEN_HEIGHT / 2;

        // set random speed on x: 1 or -1
        speedX = (random(2) == 0) ? 1 : -1;

        // set random speed on y: from -ANGLE_ADJUSTMENT_FACTOR to ANGLE_ADJUSTMENT_FACTOR
        speedY = (random(1000) / 1000.0) - ANGLE_ADJUSTMENT_FACTOR;
    }

   void draw() {
        oled.circle(static_cast<int>(round(x)), static_cast<int>(round(y)), RADIUS, OLED_FILL);
    };

    float getX() const { return x; };
    float getY() const { return y; };

    void setX(float x) { this->x = x; };
    void setY(float y) { this->y = y; };

private:
    DisplayManager &oled;
    Puddle &leftPuddle;
    Puddle &rightPuddle;

    static constexpr uint8_t RADIUS = 2;
    static constexpr float MAX_SPEED = 3.0;
    static constexpr float ANGLE_ADJUSTMENT_FACTOR = 0.5;


    float x = SCREEN_WIDTH / 2, y = SCREEN_HEIGHT / 2;
    float speedX = 0, speedY = 0;

    void increaseSpeed() {
        speedX *= 1.02;
        speedY *= 1.02;

        if (abs(speedX) > MAX_SPEED) speedX = (speedX > 0) ? MAX_SPEED : -MAX_SPEED;
        if (abs(speedY) > MAX_SPEED) speedY = (speedY > 0) ? MAX_SPEED : -MAX_SPEED;
    }

    bool checkCollisionWithWall() {
        return y + RADIUS >= SCREEN_HEIGHT || y - RADIUS <= 0;
    }

    
    bool checkCollisionWithPaddle(uint16_t paddleX, uint16_t paddleY, uint16_t paddleWidth, uint16_t paddleHeight) {
        bool collisionWithPaddleX = x + RADIUS >= paddleX && x - RADIUS <= paddleX + paddleWidth;
        bool collisionWithPaddleY = y + RADIUS >= paddleY && y - RADIUS <= paddleY + paddleHeight;

        return collisionWithPaddleX && collisionWithPaddleY;
    }

    void adjustAngleOnPaddleCollision(uint16_t paddleY, uint16_t paddleHeight) {
        float distanceFromCenter = (y - (paddleY + paddleHeight / 2)) / (paddleHeight / 2);
        speedY += distanceFromCenter * ANGLE_ADJUSTMENT_FACTOR;
    }
};

#endif
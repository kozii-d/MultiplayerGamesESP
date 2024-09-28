#ifndef PUDDLE_H
#define PUDDLE_H

enum PuddleDirection {
    UP = -1,
    DOWN = 1
};

class Puddle
{
public:
    Puddle(DisplayManager &oled, int16_t x, int16_t y, uint16_t width, uint16_t height, uint8_t stepSize) : 
    oled(oled), x(x), y(y), width(width), height(height), stepSize(stepSize) {};

    void move(PuddleDirection direction) {
        int8_t delta = direction * stepSize;
        y += delta;

        if (y < 0) y = 0;
        if (y > SCREEN_HEIGHT - height) y = SCREEN_HEIGHT - height;
    };

    void draw() {
        oled.rect(x, y, x + width - 1, y + height, OLED_FILL);
    };

    int16_t getY() const { return y; };
    int16_t getX() const { return x; };
    uint16_t getWidth() const { return width; };
    uint16_t getHeight() const { return height; };

    void setY(int16_t y) { this->y = y; };

private:
    DisplayManager &oled;
    int16_t x, y;
    uint16_t width, height;
    uint8_t stepSize;
};

#endif
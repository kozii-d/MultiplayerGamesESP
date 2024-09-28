#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#define OLED_SPI_SPEED 8000000ul
#include <GyverOLED.h>

#define OLED_CS D8
#define OLED_DC D6
#define OLED_RST D4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

class DisplayManager : public GyverOLED<SSH1106_128x64, OLED_BUFFER, OLED_SPI, OLED_CS, OLED_DC, OLED_RST> {
public:
    DisplayManager() : GyverOLED<SSH1106_128x64, OLED_BUFFER, OLED_SPI, OLED_CS, OLED_DC, OLED_RST>() {}

    void begin() {
        init();
        clear();
        update();
    }

void showMessage(const char* message, uint8_t textScale = 1) {
    clear();
    textScale = constrain(textScale, 1, 4);
    setScale(textScale);

    const uint8_t charHeight = 7 * textScale;
    const uint8_t charWidth = 5 * textScale;
    const uint8_t letterSpacing = 1 * textScale;
    const uint8_t lineSpacing = 1 * textScale;
    const uint16_t maxWidth = SCREEN_WIDTH;

    size_t messageLength = strlen(message);
    uint16_t textWidth = charWidth * messageLength + letterSpacing * (messageLength - 1);

    if (textWidth <= maxWidth) {
        drawCenteredText(message, (SCREEN_HEIGHT / 2) - (charHeight / 2), charWidth, letterSpacing);
    } else {
        size_t splitIndex = findSplitIndex(message, messageLength);
        char line1[splitIndex + 1];
        char line2[messageLength - splitIndex + 1];
        
        strncpy(line1, message, splitIndex);
        line1[splitIndex] = '\0';
        strcpy(line2, message + splitIndex + 1);

        drawCenteredText(line1, (SCREEN_HEIGHT / 2) - (charHeight + lineSpacing), charWidth, letterSpacing);
        drawCenteredText(line2, (SCREEN_HEIGHT / 2), charWidth, letterSpacing);
    }

    update();
}

private:
    void drawCenteredText(const char* text, uint8_t y, uint8_t charWidth, uint8_t letterSpacing) {
        size_t textLength = strlen(text);
        uint16_t textWidth = charWidth * textLength + letterSpacing * (textLength - 1);
        uint8_t x = (SCREEN_WIDTH / 2) - (textWidth / 2);
        setCursorXY(x, y);
        print(text);
    }

    size_t findSplitIndex(const char* message, size_t length) {
        size_t splitIndex = length / 2;
        while (splitIndex > 0 && message[splitIndex] != ' ') {
            splitIndex--;
        }
        return splitIndex == 0 ? length / 2 : splitIndex;
    }
};

#endif
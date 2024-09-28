#ifndef MENU_H
#define MENU_H

#include <vector>
#include <functional>
#include <EncButton.h>

class Menu {
public:
    Menu(DisplayManager& oled, uint8_t upButtonPin, uint8_t downButtonPin, uint8_t selectButtonPin) : 
    oled(oled), 
    upButton(upButtonPin), 
    downButton(downButtonPin), 
    selectButton(selectButtonPin), 
    currentIndex(0) 
    {}

    void addItem(const String& name, std::function<void()> action) {
        items.push_back({name, action});
    }

    void displayMenu() {
        oled.clear();
        oled.setScale(1);
        for (size_t i = 0; i < items.size(); ++i) {
            if (i == currentIndex) {
                oled.setCursor(0, i);
                oled.print(">");
            }
            oled.setCursor(10, i);
            oled.print(items[i].name);
        }
        oled.update();
    }

    void loop() {
        handleInput();
    }

private:
    struct MenuItem {
        String name;
        std::function<void()> action;
    };

    DisplayManager& oled;
    Button upButton;
    Button downButton;
    Button selectButton;
    std::vector<MenuItem> items;
    size_t currentIndex;


    void handleInput() {
        upButton.tick();
        downButton.tick();
        selectButton.tick();

        if (upButton.click()) {
            currentIndex = (currentIndex == 0) ? items.size() - 1 : currentIndex - 1;
            displayMenu();
        }

        if (downButton.click()) {
            currentIndex = (currentIndex == items.size() - 1) ? 0 : currentIndex + 1;
            displayMenu();
        }

        if (selectButton.click()) {
            items[currentIndex].action();
        }
    }

};

#endif
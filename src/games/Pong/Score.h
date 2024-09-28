#ifndef SCORE_H
#define SCORE_H

class Score {
public:
    Score(DisplayManager &oled, uint16_t left, uint16_t right) : 
    oled(oled), left(left), right(right) {}

    void draw() {
        char leftScoreText[6];  
        char rightScoreText[6];
        char scoreText[15];  

        snprintf(leftScoreText, sizeof(leftScoreText), "%02d", left);
        snprintf(rightScoreText, sizeof(rightScoreText), "%02d", right);
        snprintf(scoreText, sizeof(scoreText), "%s : %s", leftScoreText, rightScoreText);

        oled.showMessage(scoreText, 2);
    };

    void incrementLeft() {
        left++;
        draw();
    };

    void incrementRight() {
        right++;
        draw();
    };

    uint16_t getLeft() const {
        return left;
    };

    uint16_t getRight() const {
        return right;
    };

    void setLeft(uint16_t left) {
        this->left = left;
    };

    void setRight(uint16_t right) {
        this->right = right;
    };


private:
    DisplayManager &oled;
    uint16_t left, right;
};

#endif
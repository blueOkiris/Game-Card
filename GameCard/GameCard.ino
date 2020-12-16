#include "Oled.hpp"
#include "Ssd1306.hpp"

gamecard::Oled oled;

void setup() {
    //const gamecard::Ssd1306 disp;
    //disp.test();
    
    oled.init();
    //oled.testDisplay();
}

void loop() {
    gamecard::Sprite testSprite = oled.getSprite(0);
    testSprite.x++;
    if(testSprite.x > 120) {
        testSprite.x = 0;
        testSprite.y++;
    }
    if(testSprite.y > 56) {
        testSprite.y = 0;
    }
    oled.setSprite(0, testSprite);
    delay(10);
}

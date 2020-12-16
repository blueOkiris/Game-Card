#include "Oled.hpp"
#include "Ssd1306.hpp"

gamecard::Oled oled;
const uint8_t box[8] = { 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF };
const uint8_t box2[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
const uint8_t cross[8] = { 0x18, 0x18, 0x7E, 0x7E, 0x18, 0x18, 0x18, 0x18 };

void setup() {
    Serial.begin(9600);
    
    oled.init();
    //oled.testDisplay();
    oled.setTile(1, box);
    oled.setTile(2, box2);
    oled.setTile(3, cross);
    
    for(int i = 0; i < 16; i++) {
        if(i % 2 == 0) {
            oled.setBg(i, 0, 1);
            oled.setBg(i, 7, 1);
            if(i > 0 && i < 7) {
                oled.setBg(0, i, 1);
                oled.setBg(15, i, 1);
            }
        } else {
            oled.setBg(i, 0, 2);
            oled.setBg(i, 7, 2);
            if(i > 0 && i < 7) {
                oled.setBg(0, i, 2);
                oled.setBg(15, i, 2);
            }
        }
    }
    oled.updateMap();
    oled.setSprite(0, (gamecard::Sprite) { 0, 0, 3 });
    
    /*for(int j = 0; j < 16; j++) {
        for(int i = 0; i < 16; i++) {
            oled.setSprite(0, (gamecard::Sprite) { i, j, 3 });
            oled.updateSprites();
            delay(500);
        }
    }*/
}

void loop() {
    auto spr = oled.getSprite(0);
    spr.x += 2;
    if(spr.x > 119) {
        spr.x = 0;
        spr.y += 2;
    }
    if(spr.y > 55) {
        spr.y = 0;
    }
    oled.setSprite(0, spr);
    oled.updateSprites();
}

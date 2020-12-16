#include <avr/pgmspace.h>
#include "Oled.hpp"

#define CMD_SIZE    10
const uint8_t testApp[] PROGMEM = {
    'T', 1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    'S', 0, 'W', 5, 5, 1, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
};
gamecard::Oled oled;

void setup() {
    Serial.begin(9600);
    oled.init();
    
    for(int i = 0; i < sizeof(testApp) / CMD_SIZE; i++) {
        uint8_t cmd[8];
        for(int j = 0; j < CMD_SIZE; j++) {
            cmd[j] = pgm_read_byte_near(testApp + i * CMD_SIZE + j);
        }
        runCommand(cmd);
    }
}

void loop() {
}

void runCommand(uint8_t command[CMD_SIZE]) {
    Serial.print(F("Running command: { "));
    for(int i = 0; i < CMD_SIZE; i++) {
        Serial.print(command[i]);
        Serial.print(F(" "));
    }
    Serial.println(F(" }"));
    switch(command[0]) {
        case 'S':                       // Sprite
            // 0 is S
            // 1 is the index
            // 2-rest is the data
            switch(command[2]) {
                case 'W':               // Entire
                    oled.setSprite(
                        command[1], { command[3], command[4], command[5] }
                    );
                    break;
                case 'X':               // X
                    break;
                case 'Y':               // Y
                    break;
                case 'I':               // tile index
                    break;
            }
            break;
            
        case 'T':                       // Tile
            // 0 is T
            // 1 is index
            // 2-rest is data
            oled.setTile(command[1], ((uint8_t *) command) + 2);
            break;
            
        case 'U':                       // Update
            switch(command[1]) {
                case 'A':               // Update sprites and map
                    oled.updateSprites();
                    oled.updateMap();
                    break;
                case 'S':               // Update Sprites
                    oled.updateSprites();
                    break;
                case 'M':               // Update map
                    oled.updateMap();
                    break;
            }
            break;
    }
}

#include <avr/pgmspace.h>
#include "Oled.hpp"

using namespace gamecard;

#define CMD_SIZE    10
const uint8_t testApp[] PROGMEM = {
    'T', 1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    'T', 2, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF,
    
    'B', 0, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 1, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 2, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 3, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 4, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 5, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 6, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 7, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 8, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 9, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 10, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 11, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 12, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 13, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 14, 2, 0, 0, 0, 0, 0, 0, 0,
    'B', 15, 2, 0, 0, 0, 0, 0, 0, 0,
    'U', 'M', 0, 0, 0, 0, 0, 0, 0, 0,
    
    'S', 0, 'W', 5, 5, 1, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'S', 0, 'X', 'R', 2, 0, 0, 0, 0, 0,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
};
Oled oled;

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
    /*Serial.print(F("Running command: { "));
    for(int i = 0; i < CMD_SIZE; i++) {
        Serial.print(command[i]);
        Serial.print(F(" "));
    }
    Serial.println(F(" }"));*/
    switch(command[0]) {
        case 'S':                       // Sprite
            // 0 is S
            // 1 is the index
            // 2-rest is the data
            switch(command[2]) {
                case 'W':               // Entire
                    oled.sprs[command[1]] = {
                        command[3], command[4], command[5]
                    };
                    break;
                case 'X':               // X
                    oled.clearSprite(command[1]);
                    switch(command[3]) {
                        case 'R':       // Relative
                            oled.sprs[command[1]].x += command[4];
                            break;
                        case 'S':       // Set
                            oled.sprs[command[1]].x = command[4];
                            break;
                    }
                    break;
                case 'Y':               // Y
                    oled.clearSprite(command[1]);
                    switch(command[3]) {
                        case 'R':       // Relative
                            oled.sprs[command[1]].y += command[4];
                            break;
                        case 'S':       // Set
                            oled.sprs[command[1]].y = command[4];
                            break;
                    }
                    break;
                case 'I':               // Image
                    oled.clearSprite(command[1]);
                    oled.sprs[command[1]].image = command[3];
                    break;
            }
            break;
            
        case 'T':                       // Tile
            // 0 is T
            // 1 is index
            // 2-rest is data
            oled.copyTile(command[1], ((uint8_t *) command) + 2);
            break;
        
        case 'B':                       // Background
            oled.bg[command[1]] = command[2];
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

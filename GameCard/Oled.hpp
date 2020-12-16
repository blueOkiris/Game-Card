#ifndef _OLED_HPP_
#define _OLED_HPP_

#include <Arduino.h>
#include "Ssd1306.hpp"

// Some of this is limited for memory reasons
#define MAP_SIZE        128     // 128/8 * 64/8 = 16 * 8 = 128
#define MAX_SPRITES     32
#define MAX_TILES       64

namespace gamecard {
    struct Sprite {
        uint8_t x, y;
        uint8_t image;
    };
    typedef uint8_t Image[8];
    
    struct Oled {
        private:
            const Ssd1306 _display PROGMEM = Ssd1306();
        
        public:
            Image tiles[MAX_TILES];
            uint8_t bg[MAP_SIZE];
            Sprite sprs[MAX_SPRITES];
            
            void init();
            void testDisplay();
            void copyTile(uint8_t index, uint8_t data[8]);
            void clearSprite(uint8_t index);
            void updateSprites();
            void updateMap();
    };
}

#endif

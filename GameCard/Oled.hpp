#ifndef _OLED_HPP_
#define _OLED_HPP_

#include <Arduino.h>
#include "Ssd1306.hpp"

namespace gamecard {
    struct Sprite {
        uint8_t x, y;
        uint8_t image;
    };
    typedef uint8_t Image[8];
    
    struct Oled {
        private:
            Ssd1306 _display;
            
            Image _tiles[64];
            uint8_t _bg[128];
            Sprite _sprs[32];
        
        public:
            Oled();
            void updateMap();
            void updateSprites();
    };
}

#endif

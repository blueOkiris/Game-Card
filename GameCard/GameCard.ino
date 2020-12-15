#include "Ssd1306.hpp"

void setup() {
    gamecard::Ssd1306 disp(0x3C, 15);
    disp.test();
}

void loop() {
    
}

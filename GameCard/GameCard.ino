#include "Oled.hpp"
#include "Ssd1306.hpp"

gamecard::Oled oled;

void setup() {
    //const gamecard::Ssd1306 disp;
    //disp.test();
    
    oled.init();
}

void loop() {
}

/*
 * Author: Dylan Turner
 * Description: Creates instance of oled and runs the test function
 */

#include <GameCardDisplay.hpp>

const gamecard::Ssd1306 oled;

void setup() {
    oled.init();
    while(true) {
        oled.test();
    }
}

void loop() {
    // Nothing
}

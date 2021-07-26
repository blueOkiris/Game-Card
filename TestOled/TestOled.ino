/*
 * Author: Dylan Turner
 * Description: Creates instance of oled and runs the test function
 */

#include <GameCardDisplay.hpp>

void setup() {
    const gamecard::Ssd1306 oled;
    while(true) {
        oled.test();
    }
}

void loop() {
    // Nothing
}

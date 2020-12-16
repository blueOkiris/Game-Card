#include "Oled.hpp"

using namespace gamecard;

void Oled::init() {
    testDisplay();
}

void Oled::testDisplay() {
    _display.init();
    _display.test();
}

void Oled::updateMap() {
}

void Oled::updateSprites() {
    
}

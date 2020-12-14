#include "Oled.hpp"

gamecard::Oled display;

void setup() {
    
}

void loop() {
    display.updateMap();
    display.updateSprites();
}

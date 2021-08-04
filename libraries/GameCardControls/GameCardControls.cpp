/*
 * Author: Dylan Turner
 * Description: Defines implementation of input interface
 */

#include <Arduino.h>
#include <GameCardControls.hpp>

using namespace gamecard;

void ButtonController::init(void) const {
    pinMode(BTN_CONT_HIGH, INPUT);
    pinMode(BTN_CONT_LOW, INPUT);
    pinMode(BTN_CONT_UP, INPUT);
    pinMode(BTN_CONT_DOWN, INPUT);
    pinMode(BTN_CONT_LEFT, INPUT);
    pinMode(BTN_CONT_RIGHT, INPUT);
}

bool ButtonController::isInputPressed(const Input input) const {
    // Only thing to note is that U/D/L/R are all inverted
    switch(input) {
        case Input::High:
            return digitalRead(BTN_CONT_HIGH);
        
        case Input::Low:
            return digitalRead(BTN_CONT_LOW);
        
        case Input::Up:
            return !digitalRead(BTN_CONT_UP);
        
        case Input::Down:
            return !digitalRead(BTN_CONT_DOWN);
        
        case Input::Left:
            return !digitalRead(BTN_CONT_LEFT);
        
        case Input::Right:
            return !digitalRead(BTN_CONT_RIGHT);
    }
}

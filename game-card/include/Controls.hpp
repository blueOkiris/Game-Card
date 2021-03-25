/*
 * This file is for connecting to the "controller" of the game-card
 * It's for interfacing with the gpio buttons
 */
#pragma once

namespace gamecard {    
    // Gpio pins for the push buttons
    enum class ControllerInput {
        Up = 16, Down = 17, Left = 18, Right = 19,
        A = 21, B = 20
    };
    
    class Controller {
        public:
            virtual bool isPressed(const ControllerInput btn) const = 0;
    };
    
    // Implementation of a controller using just buttons
    class ButtonController : public Controller {
        public:
            ButtonController();
            bool isPressed(const ControllerInput btn) const override;
    };
}

/*
 * Author: Dylan Turner
 * Description:
 *  - Defines an interface for receiving inputs
 *  - Defines implementation of input intfc using two A & B btns & a 5-way btn
 */

#pragma once

#include <Arduino.h>

#define BTN_CONT_HIGH   9
#define BTN_CONT_LOW   10
#define BTN_CONT_UP     8
#define BTN_CONT_DOWN   7
#define BTN_CONT_LEFT   5
#define BTN_CONT_RIGHT  0

namespace gamecard {
    enum class Input { High, Low, Up, Down, Left, Right };

    class Controller {
        public:
            virtual void init(void) const = 0;
            virtual bool isInputPressed(const Input input) const = 0;
    };

    class ButtonController : public Controller {
        public:
            void init(void) const override;
            bool isInputPressed(const Input input) const override;
    };
}

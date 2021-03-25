#include <stdio.h>
#include <pico/stdlib.h>
#include <Controls.hpp>

using namespace gamecard;

ButtonController::ButtonController() {
    gpio_init(static_cast<int>(ControllerInput::Up));
    gpio_set_dir(static_cast<int>(ControllerInput::Up), false);
    gpio_pull_up(static_cast<int>(ControllerInput::Up));
    gpio_init(static_cast<int>(ControllerInput::Down));
    gpio_set_dir(static_cast<int>(ControllerInput::Down), false);
    gpio_pull_up(static_cast<int>(ControllerInput::Down));
    gpio_init(static_cast<int>(ControllerInput::Left));
    gpio_set_dir(static_cast<int>(ControllerInput::Left), false);
    gpio_pull_up(static_cast<int>(ControllerInput::Left));
    gpio_init(static_cast<int>(ControllerInput::Right));
    gpio_set_dir(static_cast<int>(ControllerInput::Right), false);
    gpio_pull_up(static_cast<int>(ControllerInput::Right));
    gpio_init(static_cast<int>(ControllerInput::A));
    gpio_set_dir(static_cast<int>(ControllerInput::A), false);
    gpio_pull_up(static_cast<int>(ControllerInput::A));
    gpio_init(static_cast<int>(ControllerInput::B));
    gpio_set_dir(static_cast<int>(ControllerInput::B), false);
    gpio_pull_up(static_cast<int>(ControllerInput::B));
}

bool ButtonController::isPressed(const ControllerInput btn) const {
    return !gpio_get(static_cast<int>(btn));
}

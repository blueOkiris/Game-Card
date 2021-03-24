#include <stdio.h>
#include <pico/stdlib.h>
#include <Controls.hpp>

using namespace gamecard;

Controller::Controller() {
    gpio_init(static_cast<int>(ControllerButton::Up));
    gpio_set_dir(static_cast<int>(ControllerButton::Up), false);
    gpio_pull_up(static_cast<int>(ControllerButton::Up));
    gpio_init(static_cast<int>(ControllerButton::Down));
    gpio_set_dir(static_cast<int>(ControllerButton::Down), false);
    gpio_pull_up(static_cast<int>(ControllerButton::Down));
    gpio_init(static_cast<int>(ControllerButton::Left));
    gpio_set_dir(static_cast<int>(ControllerButton::Left), false);
    gpio_pull_up(static_cast<int>(ControllerButton::Left));
    gpio_init(static_cast<int>(ControllerButton::Right));
    gpio_set_dir(static_cast<int>(ControllerButton::Right), false);
    gpio_pull_up(static_cast<int>(ControllerButton::Right));
    gpio_init(static_cast<int>(ControllerButton::A));
    gpio_set_dir(static_cast<int>(ControllerButton::A), false);
    gpio_pull_up(static_cast<int>(ControllerButton::A));
    gpio_init(static_cast<int>(ControllerButton::B));
    gpio_set_dir(static_cast<int>(ControllerButton::B), false);
    gpio_pull_up(static_cast<int>(ControllerButton::B));
}

bool Controller::isPressed(const ControllerButton btn) const {
    return !gpio_get(static_cast<int>(btn));
}

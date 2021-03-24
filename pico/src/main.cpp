#include <stdio.h>
#include <pico/stdlib.h>
#include <Oled.hpp>
#include <Controls.hpp>

using namespace gamecard;

void testHardware();

int main() {
    stdio_init_all();
    printf("Welcome to gamecard!\n");
    testHardware();
    return 0;
}

void testHardware() {
    const Ssd1306 oled;
    const Controller cont;
    
    oled.test();
    while(1) {
        printf("Reading controller: ");
        if(cont.isPressed(ControllerButton::Up)) {
            printf("Up pressed! ");
        }
        if(cont.isPressed(ControllerButton::Down)) {
            printf("Down pressed! ");
        }
        if(cont.isPressed(ControllerButton::Left)) {
            printf("Left pressed! ");
        }
        if(cont.isPressed(ControllerButton::Right)) {
            printf("Right pressed! ");
        }
        if(cont.isPressed(ControllerButton::A)) {
            printf("A pressed! ");
        }
        if(cont.isPressed(ControllerButton::B)) {
            printf("B pressed! ");
        }
        printf("\n");
        sleep_ms(1000);
    }
}

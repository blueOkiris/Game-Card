#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <Oled.hpp>
#include <Controls.hpp>

using namespace gamecard;

void testButtons();
void testHardware();
void testHardwareMulticore();
void testHardwareMulticoreCore1();

int main() {
    stdio_init_all();
    printf("Welcome to gamecard!\n");
    
    //testHardware();
    testHardwareMulticore();
    
    return 0;
}

void testHardwareMulticore() {
    multicore_launch_core1(testHardwareMulticoreCore1);
    const Ssd1306 oled;
    while(1) {
        oled.test();
    }
}

void testHardwareMulticoreCore1() {
    testButtons();
}

void testHardware() {
    const Ssd1306 oled;
    oled.test();
    
    testButtons();
}

void testButtons() {
    const Controller cont;
    
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

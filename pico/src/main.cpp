#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <Rom.hpp>
#include <Oled.hpp>
#include <Controls.hpp>

using namespace gamecard;

void testButtons();
void testRom();
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

// Simple test - draw to screen then read button inputs in a loop
void testHardware() {
    const Ssd1306 oled;
    oled.test();
    testRom();
    testButtons();
}

// Complex test - run oled test WHILE reading buttons with both in loop
void testHardwareMulticore() {
    multicore_launch_core1(testHardwareMulticoreCore1);
    const Ssd1306 oled;
    while(1) {
        oled.test();
    }
}

// Part of Multicore test - the part on the other core
void testHardwareMulticoreCore1() {
    sleep_ms(1000);
    printf("On second code!\n");
    sleep_ms(1000);
    testRom();
    sleep_ms(1000);
    testButtons();
}

// Write 0-100 in the SRAM and then read 0-100 back
void testRom() {
    const M23a1024 ram;
    
    printf("Writing 0-5 to SRAM\n");
    uint8_t buff[5];
    for(int i = 0; i < 5; i++) {
        buff[i] = i;
    }
    ram.write(0, buff, 5);
    sleep_ms(1000);

    for(int i = 0; i < 5; i++) {
        buff[i] = 0;
    }
    
    printf("Reading 0-5 from SRAM\n");
    ram.read(0, buff, 5);
    for(int i = 0; i < 5; i++) {
        printf("%d\n", buff[i]);
    }
    
    printf("Done.\n");
}

// Reads and prints button states in a loop
void testButtons() {
    const Controller cont;
    
    while(1) {
        printf("\r                                                           ");
        printf("                                                           \r");
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
        sleep_ms(1000);
    }
}

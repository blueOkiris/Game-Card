#include <stdio.h>
#include <memory>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <Rom.hpp>
#include <Display.hpp>
#include <Controls.hpp>
#include <VirtualMachine.hpp>

using namespace gamecard;

void testButtons();
void testRom();
void testReadRom();
void testHardware();
void testHardwareMulticore();
void testHardwareMulticoreCore1();

int main() {
    stdio_init_all();
    sleep_ms(1000);
    printf("Welcome to gamecard!\n");
    
    //testHardware();
    //testHardwareMulticore();
    testReadRom();
    
    /*printf("Connecting to hardware\n");
    const Ssd1306 oled;
    const ButtonController cont;
    const M23a1024 rom;
    
    printf("Starting virtual machine\n");
    const VirtualMachine vm(oled, cont, rom);*/
    
    return 0;
}

void testReadRom() {
    const M23a1024 rom;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 8; j++) {
            uint8_t data;
            rom.read(i * 8 + j, &data, 1);
            printf("%d ", data);
        }
        printf("\n");
    }
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
    const ButtonController cont;
    
    while(1) {
        printf("\r                                                           ");
        printf("                                                           \r");
        printf("Reading controller: ");
        if(cont.isPressed(ControllerInput::Up)) {
            printf("Up pressed! ");
        }
        if(cont.isPressed(ControllerInput::Down)) {
            printf("Down pressed! ");
        }
        if(cont.isPressed(ControllerInput::Left)) {
            printf("Left pressed! ");
        }
        if(cont.isPressed(ControllerInput::Right)) {
            printf("Right pressed! ");
        }
        if(cont.isPressed(ControllerInput::A)) {
            printf("A pressed! ");
        }
        if(cont.isPressed(ControllerInput::B)) {
            printf("B pressed! ");
        }
        sleep_ms(1000);
    }
}

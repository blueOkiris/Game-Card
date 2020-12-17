#include <avr/pgmspace.h>
#include "Device.hpp"

const uint8_t testApp[] PROGMEM = {
    'T', 1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    'T', 2, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF,
    
    'B', 'L', 0, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 1, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 2, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 3, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 4, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 5, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 6, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 7, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 8, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 9, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 10, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 11, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 12, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 13, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 14, 'L', 2, 0, 0, 0, 0, 0,
    'B', 'L', 15, 'L', 2, 0, 0, 0, 0, 0,
    'U', 'M', 0, 0, 0, 0, 0, 0, 0, 0,
    
    'S', 'L', 0, 'W', 'L', 5, 'L', 5, 'L', 1,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    
    'R', 1, 'L', 'L', 0, 0, 119, 0, 0, 0,
    'R', 0, 'L', 'L', 0, 0, 5, 0, 0, 0,
    'S', 'L', 0, 'X', 'R', 0, 'S', 0, 0, 0,
    'C', 0, 1, 0, 0, 0, 0, 0, 0, 0,
    'J', 'E', 0, 0, 0, 0, 0, 0, 0, 30,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'R', 0, 'L', 0, 0, 0, 1, 'R', 0, 0,
    'J', 'J', 0, 0, 0, 0, 0, 0, 0, 23,
};
gamecard::VirtualMachine vm;

void setup() {
    Serial.begin(9600);
    vm.init();
}

void loop() {
    uint8_t cmd[8];
    for(int j = 0; j < VM_CMD_LEN; j++) {
        cmd[j] = pgm_read_byte_near(testApp + vm.pc * VM_CMD_LEN + j);
    }
    
    vm.execute(cmd);
    
    while(vm.pc >= sizeof(testApp) / VM_CMD_LEN);
}


#include <avr/pgmspace.h>
#include "Device.hpp"

const uint8_t testApp[] PROGMEM = {
    'T', 1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    'T', 2, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF,
    
    'R', 1, 'L', 0, 0, 0, 16, 'S', 0, 0,
    'R', 0, 'L', 0, 0, 0, 0, 'S', 0, 0,
    'B', 'R', 0, 'L', 2, 0, 0, 0, 0, 0,
    'R', 0, 'L', 0, 0, 0, 1, 'R', 0, 0,
    'C', 0, 1, 0, 0, 0, 0, 0, 0, 0,
    'J', 'G', 0, 0, 0, 0, 0, 0, 0, 8,
    'J', 'J', 0, 0, 0, 0, 0, 0, 0, 3,
    'U', 'M', 0, 0, 0, 0, 0, 0, 0, 0,
    
    'S', 'L', 0, 'W', 'L', 5, 'L', 5, 'L', 1,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    
    'R', 1, 'L', 0, 0, 0, 16, 'S', 0, 0,
    'R', 0, 'L', 0, 0, 0, 5, 'S', 0, 0,
    'S', 'L', 0, 'X', 'R', 0, 'S', 0, 0, 0,
    'C', 0, 1, 0, 0, 0, 0, 0, 0, 0,
    'J', 'G', 0, 0, 0, 0, 0, 0, 0, 20,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'R', 0, 'L', 0, 0, 0, 1, 'R', 0, 0,
    'J', 'J', 0, 0, 0, 0, 0, 0, 0, 13,
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


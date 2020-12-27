#include <avr/pgmspace.h>
#include "Device.hpp"
#include "Rom.hpp"

/*const uint8_t testApp[] PROGMEM = {
    'T', 1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    'T', 2, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF,
    
    'R', 1, 'L', 0, 0, 0, 16, 'S', 0, 0,
    'R', 2, 'L', 0, 0, 0, 0, 'S', 0, 0,
    'B', 'R', 2, 'L', 2, 0, 0, 0, 0, 0,
    'R', 2, 'L', 0, 0, 0, 1, 'R', 0, 0,
    'C', 2, 1, 0, 0, 0, 0, 0, 0, 0,
    'J', 'G', 0, 0, 0, 0, 0, 0, 0, 0x08,
    'J', 'J', 0, 0, 0, 0, 0, 0, 0, 0x03,
    'U', 'M', 0, 0, 0, 0, 0, 0, 0, 0,
    
    'S', 'L', 0, 'W', 'L', 5, 'L', 5, 'L', 1,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    
    'R', 1, 'L', 0, 0, 0, 16, 'S', 0, 0,
    'R', 2, 'L', 0, 0, 0, 5, 'S', 0, 0,
    'S', 'L', 0, 'X', 'R', 2, 'S', 0, 0, 0,
    'C', 2, 1, 0, 0, 0, 0, 0, 0, 0,
    'J', 'G', 0, 0, 0, 0, 0, 0, 0, 0x16,
    'U', 'S', 0, 0, 0, 0, 0, 0, 0, 0,
    'R', 3, 'R', 0, 'S', 0, 0, 0, 0, 0,
    'R', 3, 'L', 0, 0, 0, 5, '>', 0, 0,
    'R', 2, 'R', 3, 'R', 0, 0, 0, 0, 0,
    'J', 'J', 0, 0, 0, 0, 0, 0, 0, 0x0D
};*/

gamecard::VirtualMachine vm;
const gamecard::Eeprom25LC512 rom;
uint64_t programSize;

/*
 * Here we define where to store inputs
 * Note: They are wired to port D[5:0]
 * 
 * A = D0, B = D1
 * Up = D2, Down = D3, Left = D4, and Right = D5
 *
 * vm.regs[INPUT_REG] = D & 0x3F;
 */
#define PORTD_MASK  0x3F

void setup() {
    Serial.begin(9600);
    vm.init();
    rom.init();
    
    // Set buttons as inputs by setting them all to 0
    DDRB &= ~PORTD_MASK;
    
    uint8_t sizeArr[VM_CMD_LEN];
    vm.pc++;
    rom.instruction(0, &sizeArr);
    programSize =
        (sizeArr[2] << 56) + (sizeArr[3] << 48) + (sizeArr[4] << 40)
        + (sizeArr[5] << 32) + (sizeArr[6] << 24) + (sizeArr[7] << 16)
        + (sizeArr[8] << 8) + sizeArr[9];
}

void loop() {
    uint8_t cmd[VM_CMD_LEN];
    rom.instruction(vm.pc, &cmd);
    /*for(int j = 0; j < VM_CMD_LEN; j++) {
        cmd[j] = pgm_read_byte_near(testApp + vm.pc * VM_CMD_LEN + j);
    }*/
    Serial.print("Instruction: { ");
    for(int i = 0; i < VM_CMD_LEN; i++) {
        Serial.print(cmd[i]);
        Serial.pritn(' ');
    }
    Serial.println("}");
    vm.input((~PIND) & PORTD_MASK);
    vm.execute(cmd);
    while(vm.pc >= programSize);
    //while(vm.pc >= sizeof(testApp) / VM_CMD_LEN);
}


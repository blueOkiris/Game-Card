#include <Arduino.h>
#include <SPI.h>
#include "Device.hpp"
#include "Rom.hpp"

using namespace gamecard;

void Eeprom25LC1024::init() const {
    SPI.begin();
    pinMode(ROM_SPI_CS, OUTPUT);
    digitalWrite(ROM_SPI_CS, HIGH);
}

void Eeprom25LC1024::instruction(
        uint32_t addr, uint8_t (*buffer)[VM_CMD_LEN]) const {
    digitalWrite(ROM_SPI_CS, LOW);
    
    SPI.transfer(ROM_CMD_READ);
    
    SPI.transfer((uint8_t) addr >> 16);
    SPI.transfer((uint8_t) (addr >> 8));
    SPI.transfer((uint8_t) (addr & 0x000000FF));
    
    for(int i = 0; i < 8; i++) {
        (*buffer)[i] = SPI.transfer(0x00);
    }
    
    digitalWrite(ROM_SPI_CS, HIGH);
}

#include <Arduino.h>
#include <SPI.h>
#include "Rom.hpp"

using namespace gamecard;

void Eeprom25LC512::init() const {
    SPI.begin();
    pinMode(ROM_SPI_CS, OUTPUT);
    digitalWrite(ROM_SPI_CS, HIGH);
}

uint8_t Eeprom25LC512::read(uint16_t addr) const {
    digitalWrite(ROM_SPI_CS, LOW);
    
    SPI.transfer(ROM_CMD_READ);
    SPI.transfer((uint8_t) (addr >> 8));
    SPI.transfer((uint8_t) (addr & 0x000000FF));
    
    uint8_t data;
    data = SPI.transfer(0x00);
    
    digitalWrite(ROM_SPI_CS, HIGH);
}

void Eeprom25LC512::write(uint16_t addr, uint8_t byte) const {
    digitalWrite(ROM_SPI_CS, LOW);
    
    SPI.transfer(ROM_CMD_WRITE);
    
    SPI.transfer((uint8_t) (addr >> 8));
    SPI.transfer((uint8_t) (addr & 0x000000FF));
    SPI.transfer(byte);
    
    digitalWrite(ROM_SPI_CS, HIGH);
}

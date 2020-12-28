#include <Arduino.h>
#include "Spi.hpp"
#include "Rom.hpp"

using namespace gamecard;

void Eeprom25LC512::init() const {
    DDRB |= ROM_CS_BMASK;       //pinMode(ROM_SPI_CS, OUTPUT);
    PORTB |= ROM_CS_BMASK;      //digitalWrite(ROM_SPI_CS, HIGH);
    
    _spi.init();
    delay(50);
}

uint8_t Eeprom25LC512::read(uint16_t addr) const {
    PORTB &= ~ROM_CS_BMASK;     //digitalWrite(ROM_SPI_CS, LOW);
    
    _spi.transfer(ROM_CMD_READ);
    _spi.transfer((char) (addr >> 8));
    _spi.transfer((char) addr);
    char data = _spi.transfer(0xFF);
    
    PORTB |= ROM_CS_BMASK;      //digitalWrite(ROM_SPI_CS, HIGH);
    return data;
}

void Eeprom25LC512::instruction(
        uint16_t addr, uint8_t buffer[VM_CMD_LEN]) const {
    PORTB &= ~ROM_CS_BMASK; //digitalWrite(ROM_SPI_CS, LOW);
    
    _spi.transfer(ROM_CMD_READ);
    
    int actualAddr = addr * VM_CMD_LEN;
    _spi.transfer((char) ((actualAddr) >> 8));
    _spi.transfer((char) (actualAddr));
    
    for(int i = 0; i < VM_CMD_LEN; i++) {
        buffer[i] = _spi.transfer(0xFF);
    }
        
    PORTB |= ROM_CS_BMASK;  //digitalWrite(ROM_SPI_CS, HIGH);
}

#include <Arduino.h>
#include "Spi.hpp"
#include "Rom.hpp"

using namespace tester;

void Eeprom25LC512::init() const {
    pinMode(ROM_SPI_CS, OUTPUT);
    digitalWrite(ROM_SPI_CS, HIGH);
    
    _spi.init();
}

uint8_t Eeprom25LC512::read(uint16_t addr) const {
    digitalWrite(ROM_SPI_CS, LOW);
    
    _spi.transfer(ROM_CMD_READ);
    _spi.transfer((char) (addr >> 8));
    _spi.transfer((char) addr);
    char data = _spi.transfer(0xFF);
    
    digitalWrite(ROM_SPI_CS, HIGH);
    return data;
}

void Eeprom25LC512::write(uint16_t page, uint8_t data[128]) const {
    digitalWrite(ROM_SPI_CS, LOW);
    _spi.transfer(ROM_CMD_WREN);
    digitalWrite(ROM_SPI_CS, HIGH);

    delay(1);
    
    digitalWrite(ROM_SPI_CS, LOW);
    
    _spi.transfer(ROM_CMD_WRITE);
    _spi.transfer((char) ((page * 128) >> 8));
    _spi.transfer((char) (page * 128));
    
    for(int i = 0; i < 128; i++) {
        _spi.transfer(data[i]);
    }
    
    digitalWrite(ROM_SPI_CS, HIGH);
    
    delay(10);
}

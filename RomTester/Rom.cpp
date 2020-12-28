#include <Arduino.h>
#include "Spi.hpp"
#include "Rom.hpp"

using namespace tester;

void Eeprom25LC512::init() const {
    pinMode(ROM_SPI_CS, OUTPUT);
    digitalWrite(ROM_SPI_CS, HIGH);
    
    spi.init();
}

uint8_t Eeprom25LC512::read(uint16_t addr) const {
    digitalWrite(ROM_SPI_CS, LOW);
    
    spi.transfer(ROM_CMD_READ);
    spi.transfer((char) (addr >> 8));
    spi.transfer((char) addr);
    char data = spi.transfer(0xFF);
    
    digitalWrite(ROM_SPI_CS, HIGH);
    return data;
}

void Eeprom25LC512::write(uint16_t page, uint8_t data[128]) const {
    digitalWrite(ROM_SPI_CS, LOW);
    spi.transfer(ROM_CMD_WREN);
    digitalWrite(ROM_SPI_CS, HIGH);

    delay(1);
    
    digitalWrite(ROM_SPI_CS, LOW);
    
    spi.transfer(ROM_CMD_WRITE);
    spi.transfer((char) ((page * 128) >> 8));
    spi.transfer((char) (page * 128));
    
    for(int i = 0; i < 128; i++) {
        spi.transfer(data[i]);
    }
    
    digitalWrite(ROM_SPI_CS, HIGH);
    
    delay(10);
}

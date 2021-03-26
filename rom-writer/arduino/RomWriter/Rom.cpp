#include <Arduino.h>
#include <SPI.h>
#include "Rom.hpp"

using namespace gamecard;

M23a1024::M23a1024() {
    pinMode(M23A1024_SPI_CS, OUTPUT);
    digitalWrite(M23A1024_SPI_CS, HIGH);
    
    _spi.init();
    
    digitalWrite(M23A1024_SPI_CS, HIGH);
    delay(50);
    digitalWrite(M23A1024_SPI_CS, LOW);
    delay(50);
    digitalWrite(M23A1024_SPI_CS, HIGH);
    _writeReg(M23A1024_REG_BM);
}

void M23a1024::write(
        const uint32_t addr, const uint8_t *buff, const int len) const {
    uint32_t currAddr = addr;
    uint8_t cmd[5];
    union {
        uint32_t val;
        struct {
            uint8_t lsb;
            uint8_t nlsb;
            uint8_t pmsb;
            uint8_t msb;
        };
    } inst;
    for(int i = 0; i < len; i++) {
        inst.val = (M23A1024_WRITE << 24) | (currAddr++ & 0x00FFFFFF);
        cmd[0] = inst.msb;
        cmd[1] = inst.pmsb;
        cmd[2] = inst.nlsb;
        cmd[3] = inst.lsb;
        cmd[4] = buff[i];
        Serial.print("Sending: ");
        digitalWrite(M23A1024_SPI_CS, LOW);
        for(int j = 0; j < 5; j++) {
            Serial.print(cmd[j]);
            _spi.transfer(cmd[j]);
        }
        digitalWrite(M23A1024_SPI_CS, HIGH);
        Serial.println();
    }
}

void M23a1024::_writeReg(const uint8_t data) const {
    uint8_t cmd[2] = { M23A1024_WRMR, data };
    digitalWrite(M23A1024_SPI_CS, LOW);
    _spi.transfer(cmd[0]);
    _spi.transfer(cmd[1]);
    digitalWrite(M23A1024_SPI_CS, HIGH);
}

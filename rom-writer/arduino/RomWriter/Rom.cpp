#include <Arduino.h>
#include <SPI.h>
#include "Rom.hpp"

using namespace gamecard;

M23a1024::M23a1024() {
    SPI.begin();
    pinMode(M23A1024_SPI_CS, OUTPUT);
    digitalWrite(M23A1024_SPI_CS, HIGH);
    
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
    Instruction inst;
    for(int i = 0; i < len; i++) {
        inst.val = (M23A1024_WRITE << 24) | (currAddr++ & 0x00FFFFFF);
        SPI.beginTransaction(
            SPISettings(M23A1024_SPI_SPD, MSBFIRST, SPI_MODE0)
        );
        digitalWrite(M23A1024_SPI_CS, LOW);
        SPI.transfer(inst.msb);
        SPI.transfer(inst.pmsb);
        SPI.transfer(inst.nlsb);
        SPI.transfer(inst.lsb);
        SPI.transfer(buff[i]);
        digitalWrite(M23A1024_SPI_CS, HIGH);
        SPI.endTransaction();
    }
}

void M23a1024::read(
        const uint32_t addr, uint8_t *buff, const int len) const {
    uint32_t currAddr = addr;
    Instruction inst;
    for(int i = 0; i < len; i++) {
        inst.val = (M23A1024_READ << 24) | (currAddr++ & 0x00FFFFFF);
        SPI.beginTransaction(
            SPISettings(M23A1024_SPI_SPD, MSBFIRST, SPI_MODE0)
        );
        digitalWrite(M23A1024_SPI_CS, LOW);
        SPI.transfer(inst.msb);
        SPI.transfer(inst.pmsb);
        SPI.transfer(inst.nlsb);
        SPI.transfer(inst.lsb);
        buff[i] = SPI.transfer(0x00);
        digitalWrite(M23A1024_SPI_CS, HIGH);
        SPI.endTransaction();
    }
}

void M23a1024::_writeReg(const uint8_t data) const {
    SPI.beginTransaction(SPISettings(M23A1024_SPI_SPD, MSBFIRST, SPI_MODE0));
    digitalWrite(M23A1024_SPI_CS, LOW);
    SPI.transfer(M23A1024_WRMR);
    SPI.transfer(data);
    digitalWrite(M23A1024_SPI_CS, HIGH);
    SPI.endTransaction();
}

#include <Arduino.h>
#include "Spi.hpp"

using namespace gamecard;

void SpiBus::init() const {
    pinMode(SPI_PIN_MOSI, OUTPUT);
    pinMode(SPI_PIN_MISO, INPUT);
    pinMode(SPI_PIN_SCK, OUTPUT);
  
    // Interrupt disabled, spi enabled, msb 1st, master, clock default low
    // Sample on leading edge of clock, system clock / 4 (fastest)
    SPCR = (1 << SPE) | (1 << MSTR);
  
    // Read and clear registers
    byte clr = SPSR;
    clr = SPDR;
  
    delay(10);
}

char SpiBus::transfer(volatile char data) const {
    SPDR = data;
    while(!(SPSR & (1 << SPIF)));
    return SPDR;
}

#include "Rom.hpp"

const gamecard::Eeprom25LC512 rom;
//uint32_t addr = 0;

void setup() {
    Serial.begin(9600);
    rom.init();
    
    Serial.println(F("Writing 1 through 10..."));
    for(int i = 0; i < 10; i++) {
        rom.write((uint16_t) i, (uint8_t) i);
    }
    
    Serial.println(F("Reading data back: "));
    for(int i = 0; i < 10; i++) {
        Serial.print(rom.read((uint16_t) i), HEX);
        Serial.print(F(" "));
    }
    Serial.println();
    
    /*while(addr < 65536) {
        for(int i = 0; i < 16; i++) {
            Serial.print(rom.read(addr++), HEX);
            Serial.print(" ");
        }
        Serial.println();
    }*/
}

void loop() {
}

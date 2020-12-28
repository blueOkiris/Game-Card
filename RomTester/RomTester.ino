#include "Rom.hpp"

const tester::Eeprom25LC512 rom;
int address;
uint8_t data[128];

void setup() {
    Serial.begin(9600);
    rom.init();
    address = 0;
    for(int i = 0; i < 128; i++) {
        data[i] = i;
    }
    
    Serial.println(F("Starting write cycle!"));
    for(int i = 0; i < 512; i++) {
        rom.write(i, data);
    }
    
    Serial.print(F("Starting read cycle!"));
}

void loop() {
    if(address % 128 == 0) {
        Serial.println();
        Serial.print(F("Page: "));
        Serial.print(address, HEX);
        Serial.println(F(", Data: "));
    } else if(address % 30 == 0) {
        Serial.println();
    } else if(address % 10 == 0) {
        Serial.print(F("; "));
    }
    
    uint8_t data = rom.read(address++);
    Serial.print(data, HEX);
    Serial.print(F(" "));
    delay(50);
}

#include "Rom.hpp"

uint32_t addr;
Eeprom25LC1024 rom;

void setup() {    
    Serial.begin(9600);
    rom.init();
    
    Serial.print(F("READY!"));
    bool cont = false;
    while(!cont) {
        if(Serial.available() > 0) {
            Serial.println(F("\nReceived!"));
            auto test = Serial.read();
            if(test == 0xA5) {
                cont = true;
            }
        }
    }
    
    addr = 0;
}

void loop() {
    while(Serial.available() > 0) {
        auto data = Serial.read();
        rom.write(addr++, data);
        Serial.write(data);
    }
}

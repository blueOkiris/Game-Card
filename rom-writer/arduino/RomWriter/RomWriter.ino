#include "Rom.hpp"

using namespace gamecard;

void setup() {    
    Serial.begin(9600);
    const M23a1024 rom;
    
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
    
    uint32_t addr = 0;
    while(1) {
        while(Serial.available() > 0) {
            char data = Serial.read();
            rom.write(addr, &data, 1);
            rom.read(addr, &data, 1);
            addr++;
            Serial.print(data, HEX);
        }
    }
}

void loop() { }

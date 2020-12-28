#include "Rom.hpp"

uint16_t page;
uint8_t buffer[128];
int bufferInd = 0;
gamecard::Eeprom25LC512 rom;

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
    
    page = 0;
    for(int i = 0; i < 128; i++) {
        buffer[i] = 0;
    }
}

void loop() {
    while(Serial.available() > 0) {
        buffer[bufferInd] = Serial.read();
        Serial.print(buffer[bufferInd], HEX);
        bufferInd++;
                
        if(bufferInd >= 128) {
            rom.write(page++, buffer);
            for(int i = 0; i < 128; i++) {
                buffer[i] = 0;
            }
            bufferInd = 0;
        }
    }
}

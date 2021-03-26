#include "Rom.hpp"

uint32_t page;
uint8_t buffer[128];
int bufferInd = 0;

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
    
    page = 0;
    for(int i = 0; i < 128; i++) {
        buffer[i] = 0;
    }
    
    while(1) {
        while(Serial.available() > 0) {
            buffer[bufferInd] = Serial.read();
            Serial.print(buffer[bufferInd], HEX);
            bufferInd++;
                    
            if(bufferInd >= 128) {
                rom.write(page++, buffer, 128);
                for(int i = 0; i < 128; i++) {
                    buffer[i] = 0;
                }
                bufferInd = 0;
            }
        }
    }
}

void loop() { }

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    Serial.begin(9600);
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
}

void loop() {
    while(Serial.available() > 0) {
        auto data = Serial.read();
        Serial.write(data);
    }
}

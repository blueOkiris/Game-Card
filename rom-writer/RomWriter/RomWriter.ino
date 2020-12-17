int i = 0;

void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.print(F("Hello "));
    Serial.println(i++);
    delay(1000);
}

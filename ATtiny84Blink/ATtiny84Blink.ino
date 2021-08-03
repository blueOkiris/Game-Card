/*
 * Author: Dylan Turner
 * Description:
 *  - This is based on the builtin blink
 *  - It uses pin 4 (ATtiny74's SCL) which will cause the UNO's
 *    LED to blink if you've successfully burnt the bootloader
 *    and programmed the cart
 */
void setup() {
  pinMode(4, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(4, HIGH);
  delay(1000);
  digitalWrite(4, LOW);
  delay(1000);
}

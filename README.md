# Game Card

## Description

Code for the business-card sized game console, the Game Card

## Uploading

1. Configure your Game Card to be programmed through Arduino ISP using the programming port in the order: Vcc, Gnd, Reset, Clock, MISO, MOSI.

2. Open up Arduino IDE and make sure programmer is set as "Arduino as ISP"

3. Close arduino and find your device port using `ls /dev/ttyA* && ls /dev/ttyU*`

4. If your port is `/dev/ttyACM0`, then simply run `make`. Otherwise run `PORT=<your port here> make`

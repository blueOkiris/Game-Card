# Design

## Overall

The Game Card itself is nothing but a shell containing the I2C display and buttons.
Each game can then utilize the hardware how it likes and in the most efficient way possible.

The original cartridges use ATtiny84As as the brains and ROM.

Included in this repo are games designed for the system as well as header-only libraries utilizable by the various applications.

## OLED Display Connection

The OLED display is connected through the two designated I2C pins on the ATtiny84A.
These are pins 4 (SCL) and 6 (SDA) in Arduino code or 7 and 9 on the actual device.

They are also the 5th and 6th pins on the PCB port if counting from right to left.

## Button Connection

The buttons are wired in the following order:
 - Lower Button - 3rd pin from the right on PCB (first 2 are GND then VCC) which corresponds to Arduino pin 10/ATtiny pin B0
 - Higher Button - 4th pin from the right on PCB corresponding to Arduino 9/ATtiny B1
 - Up - 7th pin from the right on PCB/Arduino 8/B2
 - Down - 8th pin from the right on PCB/Arduino 7/A7
 - Left - 9th pin from the right on PCB/Arduino 5/A5
 - Right - Last pin from the right on PCB/Arduino 0/A0

Note that H and L are pulled down, but Up/Down/Left/Right are all pulled up, so they're inverted!
Just use the library though and you'll be fine :)

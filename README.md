# Game Card

## Description

A credit card sized portable game console utilizing buttons, multiple games, and an i2c display

## Design

The Game Card itself is nothing but a shell containing the I2C display and buttons.
Each game can then utilize the hardware how it likes and in the most efficient way possible.

The original cartridges use ATtiny84As as the brains and ROM.

Included in this repo are games designed for the system as well as header-only libraries utilizable by the various applications.

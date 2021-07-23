# Game Card

## Description

A credit card sized portable game console utilizing buttons, multiple games, and an i2c display

## Design

The Game Card itself is nothing but a shell containing the I2C display and buttons.
Each game can then utilize the hardware how it likes and in the most efficient way possible.

The original cartridges use ATtiny84As as the brains and ROM.

Included in this repo are games designed for the system as well as header-only libraries utilizable by the various applications.

## Dependencies

This is how to set up your dev environment to be able to get these programs to your game cartridges.

__Set up ATtiny84:__

1) Open the IDE's preferences and add the url `https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json` to the additional boards manager urls.
2) Open the board manager and install the attiny boards.
3) Burn the bootloader using Arduino as ISP

__Set up Libraries:__

1) Download the TinyWireM library from: https://github.com/adafruit/TinyWireM
2) Install it to your Arduino's libraries folder
3) Copy the folders in the repo's libraries/ subdirectory to your Arduino libraries folder

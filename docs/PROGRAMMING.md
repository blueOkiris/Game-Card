# Programming a Cartridge

## Info

This assumes you are using the standard ATtiny84 cartridges.

This is how to set up your dev environment to be able to get these programs to those game cartridges.

## Install build tool

First, install `make`.

If you're on windows, install MinGW from [here](http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download)

That will give you access to the command `mingw32-make` which is an equivalent to `make` on Linux.

Linux users, simply use your package manager to install make. On Ubuntu/Debian, the package is within "build-essential."

After that, go to the official arduino page and install the Arduino IDE.

NOTE: If you're on Linux, you also need to change lines 6 and 7 of arduino-cli.yaml to use / instead of \\!

## Setup UNO

Open up the Arduino IDE and find the example "ArduinoISP" program in the Arduino IDE, select the UNO from the menu (noting the PORT for later), and program the UNO with the Arduino ISP program.

Once you've configured your UNO as a programmer, you won't need to open the IDE again.

## Connect Board

Once you have make, and your UNO is set up, you'll need to connect the cartridge to an Arduino UNO.

From Right to left on Programming port the connections are 5V, GND, 12, 11, 13, and 10

![prog-wiring-diagram](/docs/img/prog-wiring-diagram.jpg)

## Burn-Bootloader

With everything set up, first burn the bootloader:
 - Linux: `make burn-bootloader PORT=#####`
 - Windows: `mingw32-make burn-bootloader PORT=#####`

Replace the pound signs after `PORT=` with the UNO's port from earlier.

## Program Game

Then program the game!

 - Linux: `make PORT=##### GAME=#####`
 - Windows: `mingw32-make PORT=##### GAME=#####`

Once again, replace the pound signs after `PORT=` with the UNO's port from earlier, but also replace the pound signs after `GAME=` with the name of the game you'd like to program be that Snek, Ping, or otherwise.

And you're done!

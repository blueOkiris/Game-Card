# Programming a Cartridge

## Info

This is how to set up your dev environment to be able to get these programs to your game cartridges.

## Set up ATtiny84

1) Open the IDE's preferences and add the url `https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json` to the additional boards manager urls.

![ard-ide-men](/docs/img/arduino-ide-menu.png)

![ard-ide-pref-sel](/docs/img/arduino-ide-menu-pref-sel.png)

![ard-ide-board-man-url](/docs/img/arduino-ide-board-man-url.png)

2) Open the board manager and install the attiny boards.

![ard-ide-board-man](/docs/img/ard-ide-board-man.png)

![ard-ide-board-man-inst](/docs/img/ard-ide-board-man-inst.png)

3) Connec the cartridge to Arduino UNO. From Right to left on Programming port the connections are 5V, GND, 12, 11, 13, and 10

![prog-wiring-diagram](/docs/img/prog-wiring-diagram.jpg)

4) Find the example "ArduinoISP" program in the Arduino IDE and program the UNO

5) Switch to the ATtiny84 in the boards menu with clock option set to internal 8MHz

![ard-ide-sel-tiny84](/docs/img/ard-ide-sel-tiny84.png)

6) Select "Arduino as ISP" for the programmer and click Burn Bootloader from the menu

![ard-ide-boot-burn](/docs/img/ard-ide-boot-burn.png)

## Set up Libraries:

7) Download the TinyWireM library from the library manager
8) Copy the folders in the repo's libraries/ subdirectory to your Arduino libraries folder (Default is Documents/Arduino/libraries)

## Program Game

With everything set up:

9) Hold shift and click upload or select "upload using programmer" from the Sketch menu.

![ard-ide-prog-upld](/docs/img/ard-ide-prog-upld.png)

And you're done!

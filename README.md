# Game Card

## Description

Code for the business-card sized game console, the Game Card

## Uploading

1. Configure your Game Card to be programmed through Arduino ISP using the programming port in the order: Vcc, Gnd, Reset, Clock, MISO, MOSI.

2. Open up Arduino IDE and make sure programmer is set as "Arduino as ISP"

3. Close arduino and find your device port using `ls /dev/ttyA* && ls /dev/ttyU*`

4. If your port is `/dev/ttyACM0`, then simply run `make`. Otherwise run `PORT=<your port here> make`

## Programming Games

ROMs are programmed in hex and loaded onto cartridges which contain SPI controlled EEPROMs in the 25LC1024-I/P family. These devices are filled up with instructions with arguments of total size 10 bytes each which cause the Vritual machine to draw graphics, change memory, change the program counter, etc.

As of right now, you must manually enter the bytes using a hexeditor, however an asssembly language and assembler will be created eventually

See the instruction set for ROMs in the [instruction set document](./docs/instr.md)

Once a file is created, you can burn it to a ROM cartridge by attaching the cartridge to an Arduino UNO via SPI and using the rom-writer application.

Build the x86 app with `make rom-writer-linux-x64` and upload the rom-writer sketch to your UNO with `make writer-upload`. Note that the shared library file created is __required__ for the x86 program to run. Run the program with either `./rom-writer-linux-x64 <rom file name>` or `./rom-writer-linux-x64 <rom file name> <port name>`

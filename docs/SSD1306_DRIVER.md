# SSD1306 Custom Driver Board

## Version 4 Need

In making Version 4 of the system, I *really* wanted to source all the parts from manufacturers so that I could order boards with the parts pre-soldered, so I could feasibly sell them.

The only sourcable I2C SSD1306 I could find was [this](https://www.newark.com/midas/mdob128064v2v-yi/oled-graphic-display-cob-128x64pixels/dp/21AH3309).

It's... okay, but at almost \$14, it outclasses all other components which total \$3.00 even.
Yeah, kinda dumb.

I looked at all the major retailers, but found no luck, despite the fact you can find them at various online stores for as low as $1.25.

Solution?
Build my own driver board. It's not as bad as it seems actually. It's pretty simple.

I'm documenting this here so I can put it in one handy place for reference. It doesn't really have to do with code itself though.

## I2C Selection

These boards can communicate in a variety of ways, including Parallel, SPI, and IIC.
I've always used I2C for these throughout the project, and I want full compatibility with the current codebase (though I think I may be able to do parallel, so that is unfortunate, but it's too late).

Anyway, that selection has to be done in hardware by tying various pins together on the ribbon cable.

The datasheet is pretty helpful in this regard.

![int-sel](/docs/img/int-sel.png)

![iic-int-1](/docs/img/iic-int-1.png)

Things to note:
* Looking at the current driver board and code, I know that the SA0 bit is tied to ground as the I2C address is 0x3C.
* The R/W\# pin is obviously tied to ground because we want to write to it
* SDA\_in and SDA\_out are tied to together for one SDA
* D7:3 are tied to GND while D2:1 are tied to SDA.
* E and CS# are both low
* Reset is high bc we don't want it to reset.

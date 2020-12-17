# Instruction Set Description

## Memory Layout

The first thing to understand before programming is the sections of memory that exist. Specifically, there are four.

__Registers__

There are 32 registers available for general purpose.

These can be set, add or subtracted from, and used in various calculations

__Compare Register__

The compare register simply stores the result of a comparison. You cannot directly set it or use it, but it is used by various instructions.

Essentially, you run the compare instruction on two registers which gives you a result that's either ==, <, or >. Then you can use the result in jump instructions

__Tile Memory__

Your graphics data goes here. Every graphical item refers back to an 8x8 pixel tile in this memory.

Every byte in tile memory is a column and every 8 columns is a tile. There are a total of 64 different tiles.

You can set a tile with the tile set command. Note that you should NEVER set tile 0. It should always stay blank.

__Background Memory__

There is a 128 byte or 16x8 array called background memory.

The memory referes to the 8x8 tiles drawn across the screen, and each location in the background array (which is stored contiguously) is the index in tile memory that should be drawn at that location on the screen.

The tiles drawn by the background cannot be offset by individual pixels, they are locked to the grid of 8x8 tiles.

You can draw the background map to the screen with the update instruction and set its tile indices with the background set command

__Sprite Memory__

There is a sprite object that has a top-left corner pixel location and a tile index. There are total of 32 sprites

Unlike the background tiles, these can be drawn anywhere, but they are slower to draw, so they should be used with moving objects.

You can set them witht the sprite set command and draw them on the screen with the update command

## Instructions

Note that every instruction is composed of 10 bytes. If the command doesn't fill all 10 bytes, it must be padded with 0s

__Sprite Set__

Note that running this command will clear the selected sprite from the screen until update sprites is called again!

Begin this command with an 'S' byte

Next, select the index in sprite memory of the sprite you want to change. Either use a literal number with an 'L' byte followed by the index, or use the value stored in a register as the index with an 'R' byte followed by the register number you want to use.

Afterwards you must decide whether you want to set the whole sprite object, just its x position, just its y position, or just its tile.

* Whole Sprite:
  - Put a 'W' byte
  - Put two bytes to set x to a value, either 'L' plus a 0-255 number or 'R' plus a register index
  - Put two bytes to set y to a value, either 'L' plus a 0-255 number or 'R' plus a register index
  - Put two bytes to set the tile index to a value, either 'L' plus a 0-255 number or 'R' plus a register index
* Just the x position:
  - Put two bytes to set x to a value, either 'L' plus a 0-255 number or 'R' plus a register index.
  - Put an 'R' byte if you want to set x *relative* to your provided value, i.e. x += value, or 'S' if you want to *directly* set x to your value, i.e. x = value
* Just the y position:
  - Same as x, just for the y value
* Just the index position:
  - Put two bytes to set the tile index to a value, either 'L' plus a 0-255 number or 'R' plus a register index

Examples:

`{ 'S', 'L', 0, 'W', 'L', 5, 'L', 5, 'L', 1 }` - Sets a sprite to be { x = 5, y = 5, index = 1 }
`{ 'S', 'L', 0, 'X', 'R', 0, 'R', 0, 0, 0 }` - Sets a sprite's x position to be the sprite's x plus the value at register 0

__Tile Set__

Sets a value in tile memory to an 8x8 tile (does NOT update the screen)

This command is simple: Put a 'T', then put the index of what tile you want to set, then put 8 bytes of data.

Note that the data is columns, not rows!

If your sprite is like a 'V', for instance:
{
    0b10000001,
    0b10000001,
    0b10000001,
    0b01000010,
    0b01000010,
    0b00100100,
    0b00100100,
    0b00011000
}
It will actually show up like a '<'!

Example:
`'{ T', 2, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF }` - Sets tile 2 to be an empty square with thickness 1px

__Set Background__

Note that this command does not update the backgrounds once set!

Put a 'B' byte

Put the index in background memory. If you have (x, y), then index is (y >> 4) + x. This index, like many other things, can be either a literal value, an 'L' byte followed by the number, or the value at a register, an 'R' byte followed by the register number

Then you give it a tile index, either 'L' and a number or 'R' and a register index

Example:
`{ 'B', 'R', 0, 'L', 2, 0, 0, 0, 0, 0 }` - sets the background tile located at the index given by the value stored in register 0 to tile #2

__Update__

The update command can either draw backgrounds to the screen, draw sprites to the screen, or draw both

Put a 'U' byte and then one of the following:
 - 'A' for "all" i.e. background map then sprites
 - 'M' for just the background map
 - 'S' for just sprites

__Set Register__

Put an 'S' byte, then the number of the register you wish to set.

Then put an 'L' byte followed by a 4-byte (32-bit) number to set the data to the number, or an 'R' followed by a number to set the data to the value of another register which is indexed by said number.

Then put an 'S' to set the primary register to the data i.e. register index = data or 'R' to set the primary register *relative* to the data i.e. register index += data

Example:
`{ 'R', 1, 'L', 0, 0, 0, 16, 'S', 0, 0 }` - sets register 1 equal to 16 (remember that 0, 0, 0, 16 is really 0x00, 0x00, 0x00, 0x0F or 0x0000000F, a 32-bit integer)
`{ 'R', 0, 'R', 1, 'R', 0, 0, 0, 0, 0 }` - sets the value in register 0 equal to the value in register 0 plus the value in register 1

__Compare__

Put a 'C', then an index for register a, then an index for register b

It will then set the compare flag to Equal id reg a == reg b, Less Than if reg a \< reg b, and greater than if reg a \> reg b

Example:
`{ 'C', 0, 1, 0, 0, 0, 0, 0, 0, 0 }` - Compares the value in reg 0 to the value in reg 1

__Jump__

Jump moves the program counter to an index. Note that the program counter increases afterwards, so if you want to jump to instruction #3, set the program counter to 2 in the jump instruction

With that said, there are ways to control execution of the jump instruction. The jump instruction takes a 'J', followed by a compare byte, followed by 8 bytes forming a 64-bit unsigned integer to set the program counter to

These compare bytes make it so the jump command is conditional on the value of the compare register. They are:
- 'J' - Standard jump. Unaffected by compare register
- '=' - Jump only if the compare register is Equal
- '\<' - Jump only if the compare register is LessThan
- '\>' - Jump only if the compare register is GreaterThan
- 'L' - Jump only if the compare register is GreaterThan OR Equal
- 'G' - Jump only if the compare register is LessThan OR Equal
- '!' - Jump only if the compare register is *not* Equal

Example:
`{ 'J', 'J', 0, 0, 0, 0, 0, 0, 0, 3 }` - Jumps to instruction 3+1 = 4, regardless of compare register
<br>
Here's two instructions to show the combination of jump with the compare instruction:
```
{
    'C', 0, 1, 0, 0, 0, 0, 0, 0, 0,
    'J', 'G', 0, 0, 0, 0, 0, 0, 0, 20
}
```
Jumps to instruction 20+1 = 21 if the compare register is equal to zero, i.e. if reg 0 >= reg 1

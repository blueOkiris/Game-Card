# Assembly Programming Guide

## Description

Here I describe how to program for the Game Card in Game Card Assembly using the Game Card Assembler (gca).

See [example.gca](./example.gca) for an example of gca code

## Instructions

__Memory Control__

There are several commands for manipulating memory.

Some can set tile mem, background mem, and sprite mem, but those are the most complicated. I'll start first with registers and move from there.

The first instruction we'll look at is `mov`

`mov` takes a register indicated by "reg" followed by an index, a comma, and a value which is either an integer value, a hex value, or another register.

This command takes whatever datum follows the column and stores it in the register.

Examples:
- `mov reg0, 1237`
- `mov reg1, reg0`
- `mov reg2, 0xF78`

Adding onto that are arithmetic commands which store the value of the first register plussed or minused or whatever with the value back into the first register.

So `add reg1, reg2` is like `reg1 += reg2` or `reg1 = reg1 + reg2`

So you have `add` for `+`, `sub` for `-`, `mul` for `*`, `div` for `/`, `ls` for left shift, `rs` for right shift.

Now these can also apply to parts of sprite memory, specifically a sprites x and y position:
- `mov sprx, 0, 10`
- `add spry, reg1, 0x57`

Note:
 - There are *3* comma separated values. The first is sprx or spry, the second is which sprite you want to change, and the third is the new value as per the register versions
 - sprx and spry cannot be in the value part of the command

Even more specialized are the sprite's tile index, `spri`, a background location's tile index, `bgi`, an entire sprite, `spr`, and tiles, `tile`

__Graphical Setting__

<u>Sprites:</u>
Set an entire sprite by providing index, x, y, and then tile:

`spr <sprite index>, <x position>, <y position>, <tile index>`
All of these can be registers or discrete numbers (including hex)

Example:
`spr 0, 5, 5, 1` sets sprite 0 to be at position (5, 5) with image #1

<u>Backgrounds:</u>
`bgi` Works similarly, but with less values, just index and tile:
`bgi <bg index>, <tile index>`

Example:
`bgi reg0, 0x0F`

<u> Tiles: </u>
`tile` takes an index followed by 8 bytes representing columns

Example:
`tile 1, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF` sets the tile at index 1 to be a box

__Labels and Jumps__

The other part of the assembly language is control flow

You can set named points in a program to jump to either unconditionally or with condition.

A name plus ':' makes a label - `label_name_HERE69:`
You can then jump to it with `jmp label_nameHERE69`

These jumps can be based on conditions:
 - `je` - Jump if equal
 - `jne` - Jump if not equal
 - `jl` - Jump if less than
 - `jg` - Jump if greater than
 - `jle` - Jump if less than or equal
 - `jge` - Jump if greater than or equal

The way you set those conditions is with `cmp` (compare)

`cmp` takes two registers and compares them, setting an internal register which can be used in conditional jumps

Example:
```
label_for_loop:
    cmp reg0, reg1
    jg label_if_reg0_is_greater_than
    mov reg0, reg1
    add reg0, 1
    jmp label_for_loop
label_if_reg0_is_greater_than:
```

This code will compare reg0 to reg1, and if reg0 is NOT greater than reg1, it will set reg0 to reg1 + 1 then jump back to compare again.

If reg0 WAS greater than reg1, or the second time through where it MUST be reg0, then it will jump out of the loop to `label_if_reg0_is_greater_than`

__Comments__

Comments are anything following a ';' until a new line

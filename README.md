# Game Card

## Description

A credit card sized portable game console utilizing buttons, multiple games, and an i2c display

## Version 1

Version 1 was designed for an Arduino and is essentially complete, though I never got the PCB working.

I've now moved to version 2 which utilizes the Raspberry Pi Pico, a $4 microcontroller that greatly overpowers my $2 ATmega328p

## Building

For building the interpreter to run on a physical game card, the project creator, and the assembler:

 1. Install dotnet core 5.0

 2. Install other needed dependencies: (on Debian) run `make install-deps`

 3. Then build the interpreter, the game-writer project maker, and the game-card assembler (gca) by running `make`

For developing a game in game-card assembly:

 1. Code the game and save as a .gca file
 
 2. Build the hex code by running `gca <input-file>.gca [ -p <proj-folder> ] [ <output file> ]`

 4. To create a project for writing the game's hex file to a ROM chip, run `./rom-writer-pc <filename>.hex`

 5. Finally, to build a .uf2 for that project once created, run `make game-writer.uf2`

## Assembler Creation

There is an assembler, and it needs a parser

The language grammar is defined here (note that case is irrelevant):

```
<program>       ::= { <inst> | <label> | <include> }
<label>         ::= <ident> ':'
<inst>          ::= <cmd> <arg-list>
<include>       ::= 'include' <string>
<ident>         ::= /[a-z_][a-z0-9_]*/
<cmd>           ::= 'mov' | 'add' | 'sub' | 'mul' | 'div' | 'shr' | 'shl'
                  | 'til' | 'upd' | 'cmp' | 'del'
                  | 'jmp' | 'je'  | 'jne' | 'jgt' | 'jlt' | 'jge' | 'jle'
<arg-list>      ::= <arg> [ ',' <arg-list> ]
<arg>           ::= 'r' <arg>   | 'bg' <arg>   | 'spr' <arg>
                  | 'spx' <arg> | 'spy' <arg> | 'spi' <arg>
                  | 'sprs' | 'map' | 'gfx' | 'inp' | <integer>
<integer>       ::= /[0-9]+/ | '0b' /[01]+/ | '0x' /[0-9a-f]+/
<string>        ::= /'(\\.|[^\\\'])*'/
```

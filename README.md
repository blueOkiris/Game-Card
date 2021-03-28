# Game Card

## Description

A credit card sized portable game console utilizing buttons, multiple games, and an i2c display

## Version 1

Version 1 was designed for an Arduino and is essentially complete, though I never got the PCB working.

I've now moved to version 2 which utilizes the Raspberry Pi Pico, a $4 microcontroller that greatly overpowers my $2 ATmega328p

## Building

To install need dependencies, (on Debian) run `make install-deps`

To build the interpreter and the game-writer project maker run `make`

To create a project for writing a game to a "ROM" chip, run `./rom-writer-pc <filename>`

To build a .uf2 for that project once created, run `make game-writer.uf2`

## Assembler Creation

There is an assembler, and it needs a parser

The language grammar is defined here (note that case is irrelevant):

```
<program>       ::= { <inst> | <label> }
<label>         ::= <ident> ':'
<inst>          ::= <cmd> <arg-list>
<ident>         ::= /[a-z_][a-z0-9_]*/
<cmd>           ::= 'mov' | 'add' | 'sub' | 'mul' | 'div' | 'shr' | 'shl'
                  | 'til' | 'upd' | 'cmp' | 'del'
                  | 'jmp' | 'je'  | 'jne' | 'jgt' | 'jlt' | 'jge' | 'jle'
<arg-list>      ::= <arg> { ',' <arg> }
<arg>           ::= 'r' <arg>   | 'bg' <arg>   | 'spr' <arg>
                  | 'spx' <arg> | 'spy' <arg> | 'spi' <arg>
                  | 'sprs' | 'map' | 'gfx' | 'inp' | <integer>
<integer>       ::= /[0-9]+/ | '0b' /[01]+/ | '0x' /[0-9a-f]+/
```

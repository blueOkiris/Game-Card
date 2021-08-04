/*
 * Author: Dylan Turner
 * Description: Sprites definition for Snek. 
 */

#ifndef _SPRITES_HPP_
#define _SPRITES_HPP_

const uint8_t g_wallSprs[4][8] = {
    // Top
    {
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
    },
    
    // Bottom
    {
        0b00000011,
        0b00000011,
        0b00000011,
        0b00000011,
        0b00000011,
        0b00000011,
        0b00000011,
        0b00000011,
    },
    
    // Left
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111,
        0b11111111
    },
    
    // Right
    {
        0b11111111,
        0b11111111,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000
    },
};

const uint8_t g_snekHead[8] = {
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111
};

const uint8_t g_snekSpr[8] = {
    0b11111111,
    0b10000001,
    0b10000001,
    0b10000001,
    0b10000001,
    0b10000001,
    0b10000001,
    0b11111111
};

const uint8_t g_foodSpr[8] = {
    0b00000000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00000000
};

const uint8_t g_blankSpr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

#define FONT_W      0
#define FONT_I      1
#define FONT_N      2
#define FONT_S      3
#define FONT_E      4
#define FONT_K      5
#define FONT_D      6
#define FONT_A      7

const uint8_t PROGMEM g_letters[8 * 8] = {
    // W
    0b00000000,
    0b00111111,
    0b0100000,
    0b00111000,
    0b00111000,
    0b01000000,
    0b00111111,
    0b00000000,

    // I
    0b00000000,
    0b01000001,
    0b01000001,
    0b01111111,
    0b01000001,
    0b01000001,
    0b01000001,
    0b00000000,

    // N
    0b00000000,
    0b01111111,
    0b00000001,
    0b00001110,
    0b00110000,
    0b01000000,
    0b01111111,
    0b00000000,
    
    // S
    0b00000000,
    0b00100110,
    0b01001001,
    0b01001001,
    0b01001001,
    0b01001001,
    0b00110010,
    0b00000000,
    
    // E
    0b00000000,
    0b01111111,
    0b01001001,
    0b01001001,
    0b01000001,
    0b01000001,
    0b01000001,
    0b00000000,

    // K
    0b00000000,
    0b01111111,
    0b00001000,
    0b00010100,
    0b00010010,
    0b00100001,
    0b01000000,
    0b00000000,

    // D
    0b00000000,
    0b01111111,
    0b01000001,
    0b01000001,
    0b01000001,
    0b01000010,
    0b00111100,
    0b00000000,

    // A
    0b00000000,
    0b01111100,
    0b00010010,
    0b00010001,
    0b00010001,
    0b00010010,
    0b01111100,
    0b00000000,
    
};

const uint8_t PROGMEM g_numSprs[10 * 8] = {
    // 0
    0b00000000,
    0b00011100,
    0b00100010,
    0b01001101,
    0b01010001,
    0b00100010,
    0b00011100,
    0b00000000,
   
    // 1
    0b00000000,
    0b01000100,
    0b01000010,
    0b01000001,
    0b01111111,
    0b01000000,
    0b01000000,
    0b00000000,

    // 2
    0b00000000,
    0b01100010,
    0b01010001,
    0b01010001,
    0b01001001,
    0b01001010,
    0b01000100,
    0b00000000,

    // 3
    0b00000000,
    0b00100010,
    0b01000001,
    0b01001001,
    0b01001001,
    0b01001001,
    0b00110110,
    0b00000000,

    // 4
    0b00000000,
    0b00001111,
    0b00001000,
    0b00001000,
    0b00001000,
    0b00001000,
    0b01111111,
    0b00000000,

    // 5
    0b00000000,
    0b00100111,
    0b01000101,
    0b01000101,
    0b01000101,
    0b01000101,
    0b00111001,
    0b00000000,
    
    // 6
    0b00000000,
    0b00111110,
    0b01001001,
    0b01001001,
    0b01001001,
    0b01001001,
    0b00110010,
    0b00000000,

    // 7
    0b00000000,
    0b00000001,
    0b00000001,
    0b01100001,
    0b00010001,
    0b00001001,
    0b00000111,
    0b00000000,
    
    // 8
    0b00000000,
    0b00110110,
    0b01001001,
    0b01001001,
    0b01001001,
    0b01001001,
    0b00110110,
    0b00000000,

    // 9
    0b00000000,
    0b01001110,
    0b10010001,
    0b10010001,
    0b10010001,
    0b10010001,
    0b01111110,
    0b00000000,
};

#endif

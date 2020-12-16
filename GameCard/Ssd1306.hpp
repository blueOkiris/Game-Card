#ifndef _SSD1306_HPP_
#define _SSD1306_HPP_

#include <Arduino.h>

/*
 * This is a wrapper for communication with the SSD1306 driver chip
 * It is controlled by sending data over I2C
 * 
 * Note that clock and data require pullups and the device can be 0-16V
 * 
 * The basic properties of the device are as follows:
 * - 128x64 pixels, but ALSO the size of data ram (in bits)
 * - Data ram is further divied into 8 pages
 * - Each pixels is thus, a bit, and each page is 8 pixel rows
 * - Drawing a tile would be:
 *     + setting a tile page 
 *     + offsetting by start x
 *     + drawing a byte (watch for over)
 *     + offsetting by 128/8 = 16 bytes
 *     + drawing a byte (watch for over)
 *     + and repeating those last two over and over again
 * - Drawing a sprite would be:
 *     + Getting page of top-left tile
 *     + draw top left through top right tiles (watch for over)
 *     + adding 16 bytes and repeating through the page
 *     + increasing the page and repeating
 *     + Draw the sprite (most inefficient part)
 * - Default address should be 0x78
 */

#define SSD_CMD_SET_CONTRAST    0x81
#define SSD_CMD_OUTPUT_IS_RAM   0xA4
#define SSD_CMD_OUTPUT_NOT_RAM  0xA5
#define SSD_CMD_DISP_NORMAL     0xA6
#define SSD_CMD_DISP_INVERSE    0xA7
#define SSD_CMD_DISP_OFF        0xAE
#define SSD_CMD_DISP_ON         0xAF
#define SSD_CMD_SET_CLK_DIV     0xD5
#define SSD_CMD_SET_MULTIPLEX   0xA8
#define SSD_CMD_SET_DISP_OFFSET 0xD3
#define SSD_CMD_SET_START_LN    0x40
#define SSD_CMD_CHARGE_PUMP     0x8D
#define SSD_CMD_MEMORYMODE      0x20
#define SSD_CMD_SEGREMAP        0x01
#define SSD_CMD_COMSCAN_DEC     0xC8
#define SSD_CMD_SET_COMPINS     0xDA
#define SSD_CMD_SET_PRECHARGE   0xD9
#define SSD_CMD_SET_VCOM_DETECT 0xDB
#define SSD_CMD_SET_COL_ADDR    0x21
#define SSD_CMD_SET_PAGE_ADDR   0x22

#define SSD_SCREEN_WIDTH        128
#define SSD_SCREEN_HEIGHT       64
#define SSD_CTRL                0x00
#define SSD_DATA                0x40
#define SSD_I2C_ADDR            0x3C
#define SSD_RESET               15

namespace gamecard {
    class Ssd1306 {
        private:
            void _command(uint8_t cmd) const;
        
        public:
            void init() const;
            void test() const;
            
            void clear() const;
            void putTile(uint8_t data[8], uint8_t tileX, uint8_t tileY) const;
            void putQuadTile(
                uint8_t tiles[4][8], uint8_t topLeftX, uint8_t topLeftY
            ) const;
            void drawOffsetTile(
                uint8_t x, uint8_t y, uint8_t data[8], uint8_t bgTiles[4][8]
            ) const;
    };
}


#endif

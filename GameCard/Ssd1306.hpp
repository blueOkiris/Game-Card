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

#define SSD_CMD_SET_CONTRAST    0x81        // Don't touch
#define SSD_CMD_OUTPUT_IS_RAM   0xA4
#define SSD_CMD_OUTPUT_NOT_RAM  0xA5        // Don't touch
#define SSD_CMD_DISP_NORMAL     0xA6        // Don't touch
#define SSD_CMD_DISP_INVERSE    0xA7        // Don't touch
#define SSD_CMD_DISP_OFF        0xAE        // Don't touch
#define SSD_CMD_DISP_ON         0xAF

#define SSD_CTRL                0x80
#define SSD_DATA                0x40

namespace gamecard {
    class Ssd1306 {
        private:
            const int _iicAddr;
            uint8_t _column, _page;
            
            void _command(uint8_t cmd);
            void _command(uint8_t cmd, uint8_t *data, size_t len);
            void _putRow(uint8_t row);
        
        public:
            Ssd1306(const int iicAddr);
            
            void setColumn(uint8_t column);
            void setPage(uint8_t page);
            
            void putTile(uint8_t *data);
            void putQuadTile(uint8_t *tiles[4]);
            void drawOffsetTile(
                uint8_t x, uint8_t y, uint8_t *data, uint8_t *bgTiles[4]
            );
    };
}


#endif

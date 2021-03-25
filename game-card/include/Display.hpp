/*
 * This file defines the representations of the SSD1306 I2C Oled device
 * It also provides a list of all the various necessary commands
 * for drawig to the display
 */
#pragma once

#include <hardware/i2c.h>

// Oled driver commands
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

// Oled settings
#define SSD_SCREEN_WIDTH        128
#define SSD_SCREEN_HEIGHT       64
#define SSD_CTRL                0x00
#define SSD_DATA                0x40
#define SSD_I2C_ADDR            0x3C
#define SSD_RESET               15

// I2C settings
#define I2C_PORT                i2c0
#define I2C_DATA_PIN            4
#define I2C_CLOCK_PIN           5
#define I2C_CLOCK_SPD           1000000

namespace gamecard {
    class Display {
        public:
            virtual void fill(const bool isClear) const = 0;
            virtual void putTile(
                const uint8_t data[8],
                const uint8_t tileX, const uint8_t tileY
            ) const = 0;
            virtual void putOffsetTile(
                const uint8_t x, const uint8_t y,
                const uint8_t data[8], const uint8_t bgTiles[4][8]
            ) const = 0;
    };
    
    // Oled implementation of a display
    class Ssd1306 : public Display {
        private:
            void _command(const uint8_t cmd) const;
            
        public:
            Ssd1306();
            void test() const;
            
            void fill(const bool isClear) const override;
            void putTile(
                const uint8_t data[8],
                const uint8_t tileX, const uint8_t tileY
            ) const override;
            void putOffsetTile(
                const uint8_t x, const uint8_t y,
                const uint8_t data[8], const uint8_t bgTiles[4][8]
            ) const override;
    };
}

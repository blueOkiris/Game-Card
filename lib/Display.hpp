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
            void _command(const uint8_t cmd) const {
                uint8_t fillCmd[2] = { SSD_CTRL, cmd };
                i2c_write_blocking(I2C_PORT, SSD_I2C_ADDR, fillCmd, 2, false);
            }
            
        public:
            Ssd1306() {
                // Initialize i2c
                i2c_init(I2C_PORT, I2C_CLOCK_SPD);
                gpio_set_function(I2C_DATA_PIN, GPIO_FUNC_I2C);
                gpio_set_function(I2C_CLOCK_PIN, GPIO_FUNC_I2C);
                gpio_pull_up(4);
                gpio_pull_up(5);
                
                // Send correct commands to display to init
                _command(SSD_CMD_DISP_OFF);             // command 
                _command(SSD_CMD_SET_CLK_DIV);          // command
                _command(0x80);                         // sugg. ratio of 0x80
                _command(SSD_CMD_SET_MULTIPLEX);        // command
                _command(0x3F);
                _command(SSD_CMD_SET_DISP_OFFSET);      // command 
                _command(0x00);                         // no offset
                _command(SSD_CMD_SET_START_LN | 0x00);  // command (line 0)
                _command(SSD_CMD_CHARGE_PUMP);          // idk
                _command(0x14);                         // idk, something w pwr
                _command(SSD_CMD_MEMORYMODE);           // command
                _command(0x00);                         // 0x00 - act as ks0108
                _command(SSD_CMD_SEGREMAP | 0x01);      // idk
                _command(SSD_CMD_COMSCAN_DEC);          // idk
                _command(SSD_CMD_SET_COMPINS);          // idk
                _command(0x12);
                _command(SSD_CMD_SET_CONTRAST);         // command
                _command(0xCF);
                _command(SSD_CMD_SET_PRECHARGE);        // idk
                _command(0xF1);
                _command(SSD_CMD_SET_VCOM_DETECT);      // idk
                _command(SSD_CMD_OUTPUT_IS_RAM);        // command
                _command(SSD_CMD_DISP_NORMAL);          // command
                _command(SSD_CMD_DISP_ON);              // command
                
                fill(true);
            }
            
            void fill(const bool isClear) const override {
                uint8_t color = isClear ? 0x00 : 0xFF;
                
                // Move to top left corner
                uint8_t topLeftMove[8] = {
                    SSD_CTRL,
                        SSD_CMD_SET_PAGE_ADDR, 0, (SSD_SCREEN_HEIGHT >> 3) - 1,
                    SSD_CTRL, SSD_CMD_SET_COL_ADDR, 0, SSD_SCREEN_WIDTH - 1
                };
                i2c_write_blocking(
                    I2C_PORT, SSD_I2C_ADDR, topLeftMove, 8, false
                );
                
                // Make everything black
                uint8_t blackFill[2] = { SSD_DATA, color };
                for(int i = 0; i < 1024; i++) {
                    i2c_write_blocking(
                        I2C_PORT, SSD_I2C_ADDR, blackFill, 2, false
                    );
                }
            }

            void putTile(
                    const uint8_t data[8],
                    const uint8_t tileX, const uint8_t tileY) const override {
                // Move to the correct position
                uint8_t position_cmd[8] = {
                    SSD_CTRL, SSD_CMD_SET_PAGE_ADDR,
                    tileY, static_cast<uint8_t>(tileY + 1),
                    
                    SSD_CTRL, SSD_CMD_SET_COL_ADDR,
                    static_cast<uint8_t>(120 - (tileX << 3)),
                    static_cast<uint8_t>(127 - (tileX << 3))
                };
                i2c_write_blocking(
                    I2C_PORT, SSD_I2C_ADDR, position_cmd, 8, false
                );
                
                // Draw the tile
                uint8_t tileData[2] = { SSD_DATA, 0 };
                for(int i = 7; i >= 0; i--) {
                    tileData[1] = data[i];
                    i2c_write_blocking(
                        I2C_PORT, SSD_I2C_ADDR, tileData, 2, false
                    );
                }
            }
            void putOffsetTile(
                    const uint8_t x, const uint8_t y,
                    const uint8_t data[8],
                    const uint8_t bgTiles[4][8]) const override {
                uint8_t topLeftX = x >> 3;
                uint8_t topLeftY = (y >> 3) << 3;
                
                uint8_t offX = x - (topLeftX << 3);
                uint8_t offY = y - topLeftY;
                
                uint8_t quadTile[4][8];
                for(int i = 0; i < 4; i++) {
                    for(int j = 0; j < 8; j++) {
                        quadTile[i][j] = bgTiles[i][j];
                        
                        switch(i) {
                            case 0:
                                if(j >= offX) {
                                    quadTile[i][j] |=
                                        data[7 - (j - offX)] << offY;
                                }
                                break;
                            
                            case 1:
                                if(j < offX) {
                                    quadTile[i][j] |=
                                        data[offX - j - 1] << offY;
                                }
                                break;
                            
                            case 2:
                                if(j >= offX) {
                                    quadTile[i][j] |=
                                        data[7 - (j - offX)] >> (8 - offY);
                                }
                                break;
                                
                            case 3:
                                if(j < offX) {
                                    quadTile[i][j] |=
                                        data[offX - j - 1] >> (8 - offY);
                                }
                                break;
                        }
                    }
                }
                
                putTile(quadTile[0], topLeftX, topLeftY >> 3);
                putTile(quadTile[1], topLeftX + 1, topLeftY >> 3);
                putTile(quadTile[2], topLeftX, (topLeftY >> 3) + 1);
                putTile(quadTile[3], topLeftX + 1, (topLeftY >> 3) + 1);
            }

            void test() const {
                // Fill the screen 4 times as fast as possible
                for(int i = 0; i < 4; i++) {
                    fill(true);        
                    fill(false);
                }
                sleep_ms(1000);
                
                // Draw some sprites
                fill(true);
                uint8_t sprite[8] = {
                    0b10011001,
                    0b01100110,
                    0b00111100,
                    0b00011000,
                    0b00100100,
                    0b01000010,
                    0b10000001,
                    0b11111111,
                };
                uint8_t quadTiles[4][8] = {
                    {
                        0b11111111,
                        0b11111111,
                        0b00000011,
                        0b00000011,
                        0b00000011,
                        0b00000011,
                        0b00000011,
                        0b00000011,
                    }, {
                        0b00000011,
                        0b00000011,
                        0b00000011,
                        0b00000011,
                        0b00000011,
                        0b00000011,
                        0b11111111,
                        0b11111111,
                    }, {
                        0b11111111,
                        0b11111111,
                        0b11000000,
                        0b11000000,
                        0b11000000,
                        0b11000000,
                        0b11000000,
                        0b11000000,
                    }, {
                        0b11000000,
                        0b11000000,
                        0b11000000,
                        0b11000000,
                        0b11000000,
                        0b11000000,
                        0b11111111,
                        0b11111111,
                    }
                };
                putTile(sprite, 1, 1);
                putOffsetTile(69, 35, sprite, quadTiles);
                sleep_ms(1000);
                
                // Fill background with sprite
                fill(true);
                for(int col = 0; col < 16; col++) {
                    for(int row = 0; row < 8; row++) {
                        putTile(sprite, col, row);
                    }
                }
            }
    };
}

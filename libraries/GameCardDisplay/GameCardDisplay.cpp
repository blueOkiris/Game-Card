/*
 * Author: Dylan Turner
 * Description:
 *  - Code for Ssd1306 display implementation.
 *  - Note: requires SoftwareWire library!
 */
#include <Arduino.h>
#include <SoftwareWire.h>
#include <GameCardDisplay.hpp>

using namespace gamecard;

void Ssd1306::_command(const uint8_t cmd) const {
    _wire.beginTransmission(SSD_I2C_ADDR);
    _wire.write(SSD_CTRL);
    _wire.write(cmd);
    _wire.endTransmission();
}

void Ssd1306::init(void) const {
    // Initialize i2c
    _wire = SoftwareWire(6, 4);
    _wire.begin();
    
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

void Ssd1306::fill(const bool isClear) const {
    uint8_t color = isClear ? 0x00 : 0xFF;
    
    // Move to top left corner
    uint8_t topLeftMove[8] = {
        SSD_CTRL,
            SSD_CMD_SET_PAGE_ADDR, 0, (SSD_SCREEN_HEIGHT >> 3) - 1,
        SSD_CTRL, SSD_CMD_SET_COL_ADDR, 0, SSD_SCREEN_WIDTH - 1
    };
    _wire.beginTransmission(SSD_I2C_ADDR);
    for(int i = 0; i < 8; i++) {
        _wire.write(topLeftMove[i]);
    }
    _wire.endTransmission();
    
    // Make everything black
    for(int i = 0; i < 1024; i++) {
        _wire.beginTransmission(SSD_I2C_ADDR);
        _wire.write(SSD_DATA);
        _wire.write(color);
        _wire.endTransmission();
    }
}

void Ssd1306::putTile(
        const uint8_t data[8],
        const uint8_t tileX, const uint8_t tileY) const {
    // Move to the correct position
    uint8_t positionCmd[8] = {
        SSD_CTRL, SSD_CMD_SET_PAGE_ADDR,
        tileY, static_cast<uint8_t>(tileY + 1),
        
        SSD_CTRL, SSD_CMD_SET_COL_ADDR,
        static_cast<uint8_t>(120 - (tileX << 3)),
        static_cast<uint8_t>(127 - (tileX << 3))
    };
    _wire.beginTransmission(SSD_I2C_ADDR);
    for(int i = 0; i < 8; i++) {
        _wire.write(positionCmd[i]);
    }
    _wire.endTransmission();
    
    // Draw the tile
    for(int i = 7; i >= 0; i--) {
        _wire.beginTransmission(SSD_I2C_ADDR);
        _wire.write(SSD_DATA);
        _wire.write(data[i]);
        _wire.endTransmission();
    }
}

void Ssd1306::putOffsetTile(
        const uint8_t x, const uint8_t y,
        const uint8_t data[8],
        const uint8_t bgTiles[4][8]) const {
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

void Ssd1306::test(void) const {
    // Fill the screen 4 times as fast as possible
    for(int i = 0; i < 4; i++) {
        fill(true);        
        fill(false);
    }
    delay(1000);
    
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
    delay(1000);
    
    // Fill background with sprite
    fill(true);
    for(int col = 0; col < 16; col++) {
        for(int row = 0; row < 8; row++) {
            putTile(sprite, col, row);
        }
    }
}

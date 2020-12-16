#include <Arduino.h>
#include <Wire.h>
#include "Ssd1306.hpp"

using namespace gamecard;

Ssd1306::Ssd1306(const int iicAddr, const int resetPin) : _iicAddr(iicAddr) {
    Wire.begin();
    Wire.setClock(1000000);
    
    // Reset?
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
    delay(1);
    digitalWrite(resetPin, LOW);
    delay(10);
    digitalWrite(resetPin, HIGH);
    
    _command(SSD_CMD_DISP_OFF);             // command 
    _command(SSD_CMD_SET_CLK_DIV);          // command
    _command(0x80);                         // suggested ratio of 0x80
    _command(SSD_CMD_SET_MULTIPLEX);        // command
    _command(0x3F);
    _command(SSD_CMD_SET_DISP_OFFSET);      // command 
    _command(0x00);                         // no offset
    _command(SSD_CMD_SET_START_LN | 0x00);  // command (line 0)
    _command(SSD_CMD_CHARGE_PUMP);          // idk
    _command(0x14);                         // idk, something with power
    _command(SSD_CMD_MEMORYMODE);           // command
    _command(0x00);                         // 0x00 - act like ks0108
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
    
    clear();
}

void Ssd1306::_command(uint8_t cmd) const {
    Wire.beginTransmission(_iicAddr);
    Wire.write(SSD_CTRL);
    Wire.write(cmd);
    Wire.endTransmission();
}

void Ssd1306::clear() const {
    Wire.beginTransmission(_iicAddr);
    
    Wire.write(SSD_CTRL);
    Wire.write(SSD_CMD_SET_PAGE_ADDR);
    Wire.write(0);
    Wire.write((SSD_SCREEN_HEIGHT >> 3) - 1);
    
    Wire.write(SSD_CTRL);
    Wire.write(SSD_CMD_SET_COL_ADDR);
    Wire.write(0);
    Wire.write(SSD_SCREEN_WIDTH - 1);
    
    Wire.endTransmission(_iicAddr);
    
    for(int i = 0; i < 1024; i++) {
        Wire.beginTransmission(_iicAddr);
        Wire.write(SSD_DATA);
        Wire.write(0x00);
        Wire.endTransmission(_iicAddr);
    }
}

// Note that tiles are column bytes, not row bytes!
// If your tile was V, then it would draw >
void Ssd1306::putTile(uint8_t data[8], uint8_t tileX, uint8_t tileY) const {
    Wire.beginTransmission(_iicAddr);
    
    Wire.write(SSD_CTRL);
    Wire.write(SSD_CMD_SET_PAGE_ADDR);
    Wire.write(tileY);
    Wire.write(tileY + 1);
    
    Wire.write(SSD_CTRL);
    Wire.write(SSD_CMD_SET_COL_ADDR);
    Wire.write(120 - (tileX << 3));
    Wire.write(127 - (tileX << 3));
    
    Wire.endTransmission();
    
    for(int i = 7; i >= 0; i--) {
        Wire.beginTransmission(_iicAddr);
        Wire.write(SSD_DATA);
        Wire.write(data[i]);
        Wire.endTransmission(_iicAddr);
    }
}

void Ssd1306::putQuadTile(
        uint8_t tiles[4][8], uint8_t topLeftX, uint8_t topLeftY) const {
    Wire.beginTransmission(_iicAddr);
    
    Wire.write(SSD_CTRL);
    Wire.write(SSD_CMD_SET_PAGE_ADDR);
    Wire.write(topLeftY);
    Wire.write(topLeftY + 2);
    
    Wire.write(SSD_CTRL);
    Wire.write(SSD_CMD_SET_COL_ADDR);
    Wire.write(112 - (topLeftX << 3));
    Wire.write(127 - (topLeftX << 3));
    
    Wire.endTransmission();
    
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 8; j++) {
            Wire.beginTransmission(_iicAddr);
            Wire.write(SSD_DATA);
            Wire.write(tiles[i][j]);
            Wire.endTransmission(_iicAddr);
        }
    }
}

void Ssd1306::drawOffsetTile(
        uint8_t x, uint8_t y, uint8_t data[8], uint8_t bgTiles[4][8]) const {
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
                        quadTile[i][j] |= data[7 - (j - offX)] << offY;
                    }
                    break;
                
                case 1:
                    if(j < offX) {
                        quadTile[i][j] |= data[offX - j - 1] << offY;
                    }
                    break;
                
                case 2:
                    if(j >= offX) {
                        quadTile[i][j] |= data[7 - (j - offX)] >> (8 - offY);
                    }
                    break;
                    
                case 3:
                    if(j < offX) {
                        quadTile[i][j] |= data[offX - j - 1] >> (8 - offY);
                    }
                    break;
            }
        }
    }
    
    putQuadTile(quadTile, topLeftX, topLeftY);
}

void Ssd1306::test() const {
    // Fill the screen
    for(int i = 0; i < 4; i++) {
        clear();
        
        Wire.beginTransmission(_iicAddr);
        
        Wire.write(SSD_CTRL);
        Wire.write(SSD_CMD_SET_COL_ADDR);
        Wire.write(0);
        Wire.write(SSD_SCREEN_WIDTH - 1);
        
        Wire.write(SSD_CTRL);
        Wire.write(SSD_CMD_SET_PAGE_ADDR);
        Wire.write(0);
        Wire.write(SSD_SCREEN_HEIGHT / 8 - 1);
        
        Wire.endTransmission(_iicAddr);
        
        for(int i = 0; i < 1024; i++) {
            Wire.beginTransmission(_iicAddr);
            Wire.write(SSD_DATA);
            Wire.write(0xFF);
            Wire.endTransmission(_iicAddr);
        }
    }
    
    // Draw a tile
    clear();
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
    putQuadTile(quadTiles, 2, 2);
    drawOffsetTile(69, 2, sprite, quadTiles);
}

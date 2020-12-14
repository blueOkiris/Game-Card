#include <Arduino.h>
#include <Wire.h>
#include "Ssd1306.hpp"

using namespace gamecard;

Ssd1306::Ssd1306(const int iicAddr) : _iicAddr(iicAddr), _column(0), _page(0) {
    Wire.begin();
    _command(SSD_CMD_OUTPUT_IS_RAM);
    _command(SSD_CMD_DISP_ON);
}

void Ssd1306::_command(uint8_t cmd) {
    Wire.beginTransmission(_iicAddr);
    Wire.write(SSD_CTRL);
    Wire.write(cmd);
    Wire.endTransmission();
}

void Ssd1306::_command(uint8_t cmd, uint8_t *data, size_t len) {
    Wire.beginTransmission(_iicAddr);
    Wire.write(SSD_CTRL);
    Wire.write(cmd);
    for(size_t ind = 0; ind < len; ind++) {
        Wire.write(data[ind]);
    }
    Wire.endTransmission();
}

void Ssd1306::setColumn(uint8_t column) {
    Wire.beginTransmission(_iicAddr);
    Wire.write(SSD_CTRL);
    Wire.write(column & 0x0F);
    Wire.endTransmission();
    
    Wire.beginTransmission(_iicAddr);
    Wire.write(SSD_CTRL);
    Wire.write(0x01 + (column >> 4));
    Wire.endTransmission();
    
    _column = column;
}

void Ssd1306::setPage(uint8_t page) {
    Wire.beginTransmission(_iicAddr);
    Wire.write(SSD_CTRL);
    Wire.write(0xB0 + (page & 0x07));
    Wire.endTransmission();
    _page = page;
}

void Ssd1306::_putRow(uint8_t row) {
    Wire.beginTransmission(_iicAddr);
    Wire.write(SSD_DATA);
    Wire.write(row);
    Wire.endTransmission(_iicAddr);
}

void Ssd1306::putTile(uint8_t *data) {
    uint8_t oldCol = _column;
    for(int i = 0; i < 8; i++) {
        _putRow(data[i]);
        setColumn(_column + 15);       // (128/8)-1=16-1=15 thus next row
    }
    setColumn(oldCol);
}

void Ssd1306::putQuadTile(uint8_t *tiles[4]) {
    putTile(tiles[0]);                  // Top left
    putTile(tiles[1]);                  // Top right
    setPage(_page + 1);
    putTile(tiles[3]);                  // Bottom right
    setColumn(_column - 2);
    putTile(tiles[2]);                 // Bottom left
    setColumn(_column - 1);
    setPage(_page - 1);
}

void Ssd1306::drawOffsetTile(
        uint8_t x, uint8_t y, uint8_t *data, uint8_t *bgTiles[4]) {
    uint8_t oldCol = _column, oldPage = _page;
    
    uint8_t topLeftX = (x >> 3) << 3;   // (x / 8) * 8
    uint8_t topLeftY = (y >> 3) << 3;
    putQuadTile(bgTiles);
    
    uint8_t offX = x - topLeftX;
    uint8_t offY = y - topLeftY;
    
    for(int i = 0; i < offY; i++) {
        _putRow(data[i] >> offX);
        if(topLeftX != 15) {
            _putRow(data[i] << offX);
            setColumn(topLeftX + 14);
        } else {
            setColumn(topLeftX + 15);
        }
    }
    setColumn(topLeftX);
    setPage(_page + 1);
    for(int i = offY; i < 8; i++) {
        _putRow(data[i] >> offX);
        if(topLeftX != 15) {
            _putRow(data[i] << offX);
            setColumn(topLeftX + 14);
        } else {
            setColumn(topLeftX + 15);
        }
    }
    
    setColumn(oldCol);
    setPage(oldPage);
}

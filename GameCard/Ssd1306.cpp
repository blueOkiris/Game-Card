#include <Arduino.h>
#include <Wire.h>
#include "Ssd1306.hpp"

using namespace gamecard;

Ssd1306::Ssd1306(const int iicAddr, const int resetPin) :
        _iicAddr(iicAddr), _column(0), _page(0) {
    Wire.begin();
    
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
}

void Ssd1306::_command(uint8_t cmd) {
    Wire.beginTransmission(_iicAddr);
    Wire.write(SSD_CTRL);
    Wire.write(cmd);
    Wire.endTransmission();
}

void Ssd1306::setColumn(uint8_t column) {
    _command(SSD_CMD_SET_COL_ADDR);
    _command(column);
    _command(column + 1);    
    _column = column;
}

void Ssd1306::setPage(uint8_t page) {
    _command(SSD_CMD_SET_PAGE_ADDR);
    _command(page);
    _command(page + 1);
    
    _page = page;
}

void Ssd1306::putCol(uint8_t row) {
    Wire.beginTransmission(_iicAddr);
    Wire.write(SSD_DATA);
    Wire.write(row);
    Wire.endTransmission(_iicAddr);
}

void Ssd1306::putTile(uint8_t *data) {
    /*int8_t oldCol = _column;
    for(int i = 0; i < 8; i++) {
        putCol(data[i]);
        setColumn(_column + 15);            // (128/8)-1=16-1=15 thus next row
    }
    setColumn(oldCol);*/
}

void Ssd1306::putQuadTile(uint8_t *tiles[4]) {
    /*putTile(tiles[0]);                      // Top left
    putTile(tiles[1]);                      // Top right
    setPage(_page + 1);
    putTile(tiles[3]);                      // Bottom right
    setColumn(_column - 2);
    putTile(tiles[2]);                      // Bottom left
    setColumn(_column - 1);
    setPage(_page - 1);*/
}

void Ssd1306::drawOffsetTile(
        uint8_t x, uint8_t y, uint8_t *data, uint8_t *bgTiles[4]) {
    /*uint8_t oldCol = _column, oldPage = _page;
    
    uint8_t topLeftX = (x >> 3) << 3;       // (x / 8) * 8
    uint8_t topLeftY = (y >> 3) << 3;
    putQuadTile(bgTiles);
    
    uint8_t offX = x - topLeftX;
    uint8_t offY = y - topLeftY;
    
    for(int i = 0; i < offY; i++) {
        putCol(data[i] >> offX);
        if(topLeftX != 15) {
            putCol(data[i] << offX);
            setColumn(topLeftX + 14);
        } else {
            setColumn(topLeftX + 15);
        }
    }
    setColumn(topLeftX);
    setPage(_page + 1);
    for(int i = offY; i < 8; i++) {
        putCol(data[i] >> offX);
        if(topLeftX != 15) {
            putCol(data[i] << offX);
            setColumn(topLeftX + 14);
        } else {
            setColumn(topLeftX + 15);
        }
    }
    
    setColumn(oldCol);
    setPage(oldPage);*/
}

void Ssd1306::test() {
    Serial.begin(9600);
    for(int i = 0; i < 8; i++) {
        setPage(i);
        for(int j = 0; j < 128; j++) {
            setColumn(j);
            putCol(0x00);
        }
    }
    for(int i = 0; i < 8; i++) {
        setPage(i);
        Serial.print(F("Page: "));
        Serial.print(i);
        Serial.println();
        for(int j = 0; j < 128; j++) {
            setColumn(j);
            putCol(0xFF);
            //delay(10);
        }
    }
}

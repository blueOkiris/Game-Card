#include <Arduino.h>
#include <Wire.h>
#include "Device.hpp"

using namespace gamecard;

void Ssd1306::init() const {
    Wire.begin();
    Wire.setClock(1000000);
    
    // Reset?
    pinMode(SSD_RESET, OUTPUT);
    digitalWrite(SSD_RESET, HIGH);
    delay(1);
    digitalWrite(SSD_RESET, LOW);
    delay(10);
    digitalWrite(SSD_RESET, HIGH);
    
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
    Wire.beginTransmission(SSD_I2C_ADDR);
    Wire.write(SSD_CTRL);
    Wire.write(cmd);
    Wire.endTransmission();
}

void Ssd1306::clear() const {
    Wire.beginTransmission(SSD_I2C_ADDR);
    
    Wire.write(SSD_CTRL);
    Wire.write(SSD_CMD_SET_PAGE_ADDR);
    Wire.write(0);
    Wire.write((SSD_SCREEN_HEIGHT >> 3) - 1);
    
    Wire.write(SSD_CTRL);
    Wire.write(SSD_CMD_SET_COL_ADDR);
    Wire.write(0);
    Wire.write(SSD_SCREEN_WIDTH - 1);
    
    Wire.endTransmission(SSD_I2C_ADDR);
    
    for(int i = 0; i < 1024; i++) {
        Wire.beginTransmission(SSD_I2C_ADDR);
        Wire.write(SSD_DATA);
        Wire.write(0x00);
        Wire.endTransmission(SSD_I2C_ADDR);
    }
}

// Note that tiles are column bytes, not row bytes!
// If your tile was V, then it would draw >
void Ssd1306::putTile(uint8_t data[8], uint8_t tileX, uint8_t tileY) const {
    Wire.beginTransmission(SSD_I2C_ADDR);
    
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
        Wire.beginTransmission(SSD_I2C_ADDR);
        Wire.write(SSD_DATA);
        Wire.write(data[i]);
        Wire.endTransmission(SSD_I2C_ADDR);
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
    
    putTile(quadTile[0], topLeftX, topLeftY >> 3);
    putTile(quadTile[1], topLeftX + 1, topLeftY >> 3);
    putTile(quadTile[2], topLeftX, (topLeftY >> 3) + 1);
    putTile(quadTile[3], topLeftX + 1, (topLeftY >> 3) + 1);
}

void Ssd1306::test() const {
    // Fill the screen
    for(int i = 0; i < 4; i++) {
        clear();
        
        Wire.beginTransmission(SSD_I2C_ADDR);
        
        Wire.write(SSD_CTRL);
        Wire.write(SSD_CMD_SET_COL_ADDR);
        Wire.write(0);
        Wire.write(SSD_SCREEN_WIDTH - 1);
        
        Wire.write(SSD_CTRL);
        Wire.write(SSD_CMD_SET_PAGE_ADDR);
        Wire.write(0);
        Wire.write(SSD_SCREEN_HEIGHT / 8 - 1);
        
        Wire.endTransmission(SSD_I2C_ADDR);
        
        for(int j = 0; j < 1024; j++) {
            Wire.beginTransmission(SSD_I2C_ADDR);
            Wire.write(SSD_DATA);
            Wire.write(0xFF);
            Wire.endTransmission(SSD_I2C_ADDR);
        }
    }
    delay(1000);
    
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
    drawOffsetTile(69, 35, sprite, quadTiles);
    
    delay(1000);
    
    // Fill a background
    clear();
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 8; j++) {
            putTile(sprite, i, j);
            delay(10);
        }
    }
    
    delay(1000);
    clear();
}

void VirtualMachine::init() {
    _display.init();
    _display.clear();
    
    for(int i = 0; i < VM_MAP_SIZE; i++) {
        if(i < VM_MAX_TILES) {
            for(int j = 0; j < 8; j++) {
                _tiles[i][j] = 0;
            }
        }
        if(i < VM_MAX_SPRITES) {
            _sprs[i] = (Sprite) { 0, 0, 0 };
        }
        _bg[i] = 0;
    }
    pc = 0;
}

void VirtualMachine::_clearSprite(uint8_t index) {
    uint8_t row = _sprs[index].y >> 3;
    uint8_t col = _sprs[index].x >> 3;
    _display.putTile(_tiles[_bg[(row << 4) + col]], col, row);
    _display.putTile(_tiles[_bg[(row << 4) + col + 1]], col + 1, row);
    _display.putTile(_tiles[_bg[((row + 1) << 4) + col]], col, row + 1);
    _display.putTile(_tiles[_bg[((row + 1) << 4) + col + 1]], col + 1, row + 1);
}

void VirtualMachine::_copyTile(uint8_t index, uint8_t data[8]) {
    for(int i = 0; i < 8; i++) {
        _tiles[index][i] = data[i];
    }
}

void VirtualMachine::testDisplay() {
    _display.test();
}

void VirtualMachine::_updateMap() {
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 16; col++) {
            _display.putTile(_tiles[_bg[(row << 4) + col]], col, row);
        }
    }
}

void VirtualMachine::_updateSprites() {
    for(int i = 0; i < VM_MAX_SPRITES; i++) {
        if(_sprs[i].image != 0) {
            uint8_t row = _sprs[i].y >> 3;
            uint8_t col = _sprs[i].x >> 3;
            
            uint8_t bgTiles[4][8];
            for(int j = 0; j < 8; j++) {
                bgTiles[0][j] = _tiles[_bg[(row << 4) + col]][j];
                bgTiles[1][j] = _tiles[_bg[(row << 4) + col + 1]][j];
                bgTiles[2][j] = _tiles[_bg[((row + 1) << 4) + col]][j];
                bgTiles[3][j] = _tiles[_bg[((row + 1) << 4) + col + 1]][j];
            }
            _display.drawOffsetTile(
                _sprs[i].x, _sprs[i].y, _tiles[_sprs[i].image], bgTiles
            );
        }
    }
}

void VirtualMachine::execute(uint8_t command[VM_CMD_LEN]) {
    switch(command[0]) {
        case 'S':                       // Sprite
            _clearSprite(command[1]);
            // 0 is S
            // 1 is the index
            // 2-rest is the data
            switch(command[2]) {
                case 'W':               // Entire
                    _sprs[command[1]] = {
                        command[3], command[4], command[5]
                    };
                    break;
                case 'X':               // X
                    switch(command[3]) {
                        case 'R':       // Relative
                            _sprs[command[1]].x += command[4];
                            break;
                        case 'S':       // Set
                            _sprs[command[1]].x = command[4];
                            break;
                    }
                    break;
                case 'Y':               // Y
                    switch(command[3]) {
                        case 'R':       // Relative
                            _sprs[command[1]].y += command[4];
                            break;
                        case 'S':       // Set
                            _sprs[command[1]].y = command[4];
                            break;
                    }
                    break;
                case 'I':               // Image
                    _sprs[command[1]].image = command[3];
                    break;
            }
            break;
            
        case 'T':                       // Tile
            // 0 is T
            // 1 is index
            // 2-rest is data
            _copyTile(command[1], ((uint8_t *) command) + 2);
            break;
        
        case 'B':                       // Background
            _bg[command[1]] = command[2];
            break;
        
        case 'U':                       // Update
            switch(command[1]) {
                case 'A':               // Update sprites and map
                    _updateSprites();
                    _updateMap();
                    break;
                case 'S':               // Update Sprites
                    _updateSprites();
                    break;
                case 'M':               // Update map
                    _updateMap();
                    break;
            }
            break;
    }
    
    pc++;
}


#include "Oled.hpp"

using namespace gamecard;

void Oled::init() {
    _display.init();
    _display.clear();
    
    for(int i = 0; i < 128; i++) {
        if(i < MAX_TILES) {
            for(int j = 0; j < 8; j++) {
                _tiles[i][j] = 0;
            }
        }
        if(i < MAX_SPRITES) {
            _sprs[i] = (Sprite) { 0, 0, 0 };
        }
        _bg[i] = 0;
    }
}

Sprite Oled::getSprite(uint8_t index) {
    return _sprs[index];
}

void Oled::setSprite(uint8_t index, Sprite data) {
    // Clear old spot
    uint8_t row = _sprs[index].y >> 3;
    uint8_t col = _sprs[index].x >> 3;
    _display.putTile(_tiles[_bg[(row << 4) + col]], col, row);
    _display.putTile(_tiles[_bg[(row << 4) + col + 1]], col + 1, row);
    _display.putTile(_tiles[_bg[((row + 1) << 4) + col]], col, row + 1);
    _display.putTile(_tiles[_bg[((row + 1) << 4) + col + 1]], col + 1, row + 1);
    
    // Update
    _sprs[index] = data;
}

uint8_t Oled::getBg(uint8_t x, uint8_t y) {
    return _bg[(y << 4) + x];
}

void Oled::setBg(uint8_t x, uint8_t y, uint8_t index) {
    _bg[(y << 4) + x] = index;
}

void Oled::setTile(uint8_t index, uint8_t data[8]) {
    for(int i = 0; i < 8; i++) {
        _tiles[index][i] = data[i];
    }
}

void Oled::testDisplay() {
    _display.test();
}

void Oled::updateMap() {
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 16; col++) {
            _display.putTile(_tiles[_bg[(row << 4) + col]], col, row);
        }
    }
}

void Oled::updateSprites() {
    for(int i = 0; i < MAX_SPRITES; i++) {
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

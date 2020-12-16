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
    
    _updateMap();
}

Sprite Oled::getSprite(uint8_t index) {
    return _sprs[index];
}

void Oled::setSprite(uint8_t index, Sprite data) {
    uint8_t bgTiles[4][8];
    uint8_t row = _sprs[index].y << 3, col = _sprs[index].x >> 3;
    for(int i = 0; i < 8; i++) {
        bgTiles[0][i] = _tiles[_bg[(row >> 4) + col]][i];
        bgTiles[1][i] = _tiles[_bg[(row >> 4) + col + 1]][i];
        bgTiles[2][i] = _tiles[_bg[((row + 1) >> 4) + col]][i];
        bgTiles[3][i] = _tiles[_bg[((row + 1) >> 4) + col + 1]][i];
    }
    _display.putQuadTile(bgTiles, col, row);
    _sprs[index] = data;
    _updateSprites();
}

uint8_t Oled::getBg(uint8_t x, uint8_t y) {
    return _bg[(y >> 4) + x];
}

void Oled::setBg(uint8_t x, uint8_t y, uint8_t index) {
    _bg[(y >> 4) + x] = index;
    _updateMap();
    _updateSprites();
}

void Oled::setTile(uint8_t index, uint8_t data[8]) {
    for(int i = 0; i < 8; i++) {
        _tiles[index][i] = data[i];
    }
    _updateMap();
    _updateSprites();
}

void Oled::testDisplay() {
    _display.test();
}

void Oled::_updateMap() {
    for(int row = 0; row < 8; row += 2) {
        for(int col = 0; col < 16; col += 2) {
            uint8_t tiles[4][8];
            for(int i = 0; i < 8; i++) {
                tiles[0][i] = _tiles[_bg[(row >> 4) + col]][i];
                tiles[1][i] = _tiles[_bg[(row >> 4) + col + 1]][i];
                tiles[2][i] = _tiles[_bg[((row + 1) >> 4) + col]][i];
                tiles[3][i] = _tiles[_bg[((row + 1) >> 4) + col + 1]][i];
            }
            _display.putQuadTile(tiles, col, row);
        }
    }
}

void Oled::_updateSprites() {
    for(int i = 0; i < MAX_SPRITES; i++) {
        if(_sprs[i].image != 0) {
            uint8_t bgTiles[4][8];
            uint8_t row = _sprs[i].y >> 3, col = _sprs[i].x >> 3;
            for(int i = 0; i < 8; i++) {
                bgTiles[0][i] = _tiles[_bg[(row >> 4) + col]][i];
                bgTiles[1][i] = _tiles[_bg[(row >> 4) + col + 1]][i];
                bgTiles[2][i] = _tiles[_bg[((row + 1) >> 4) + col]][i];
                bgTiles[3][i] = _tiles[_bg[((row + 1) >> 4) + col + 1]][i];
            }
            _display.drawOffsetTile(
                _sprs[i].x, _sprs[i].y, _tiles[_sprs[i].image], bgTiles
            );
        }
    }
}

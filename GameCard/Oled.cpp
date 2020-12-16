#include "Oled.hpp"

using namespace gamecard;

void Oled::init() {
    _display.init();
    _display.clear();
    
    for(int i = 0; i < 128; i++) {
        if(i < MAX_TILES) {
            for(int j = 0; j < 8; j++) {
                tiles[i][j] = 0;
            }
        }
        if(i < MAX_SPRITES) {
            sprs[i] = (Sprite) { 0, 0, 0 };
        }
        bg[i] = 0;
    }
}

void Oled::clearSprite(uint8_t index) {
    uint8_t row = sprs[index].y >> 3;
    uint8_t col = sprs[index].x >> 3;
    _display.putTile(tiles[bg[(row << 4) + col]], col, row);
    _display.putTile(tiles[bg[(row << 4) + col + 1]], col + 1, row);
    _display.putTile(tiles[bg[((row + 1) << 4) + col]], col, row + 1);
    _display.putTile(tiles[bg[((row + 1) << 4) + col + 1]], col + 1, row + 1);
}

void Oled::copyTile(uint8_t index, uint8_t data[8]) {
    for(int i = 0; i < 8; i++) {
        tiles[index][i] = data[i];
    }
}

void Oled::testDisplay() {
    _display.test();
}

void Oled::updateMap() {
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 16; col++) {
            _display.putTile(tiles[bg[(row << 4) + col]], col, row);
        }
    }
}

void Oled::updateSprites() {
    for(int i = 0; i < MAX_SPRITES; i++) {
        if(sprs[i].image != 0) {
            uint8_t row = sprs[i].y >> 3;
            uint8_t col = sprs[i].x >> 3;
            
            uint8_t bgTiles[4][8];
            for(int j = 0; j < 8; j++) {
                bgTiles[0][j] = tiles[bg[(row << 4) + col]][j];
                bgTiles[1][j] = tiles[bg[(row << 4) + col + 1]][j];
                bgTiles[2][j] = tiles[bg[((row + 1) << 4) + col]][j];
                bgTiles[3][j] = tiles[bg[((row + 1) << 4) + col + 1]][j];
            }
            _display.drawOffsetTile(
                sprs[i].x, sprs[i].y, tiles[sprs[i].image], bgTiles
            );
        }
    }
}

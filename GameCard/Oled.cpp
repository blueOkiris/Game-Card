#include "Oled.hpp"

using namespace gamecard;

Oled::Oled() : _display(0x3C, 15) {
    for(int i = 0; i < MAP_SIZE; i++) {
        if(i < MAX_TILES) {
            for(int j = 0; j < 8; j++) {
                _tiles[i][j] = 1;
            }
        }
        _bg[i] = 0;
        if(i < MAX_SPRITES) {
            _sprs[i] = (Sprite) { 0, 0, 0 };
        }
    }
}

void Oled::updateMap() {
    for(int row = 0; row < 8; row += 2) {
        _display.setPage(row);
        for(int col = 0; col < 16; col += 2) {
            _display.setColumn(col);
            uint8_t *tiles[4] = {
                _tiles[_bg[(row << 4) + col]],
                _tiles[_bg[(row << 4) + col + 1]],
                _tiles[_bg[((row + 1) << 4) + col]],
                _tiles[_bg[((row + 1) << 4) + col + 1]]
            };
            _display.putQuadTile(tiles);
        }
    }
}

void Oled::updateSprites() {
    for(int i = 0; i < MAX_SPRITES; i++) {
        uint8_t topLeftX = (_sprs[i].x << 8) >> 8;
        uint8_t topLeftY = (_sprs[i].y << 8) >> 8;
        uint8_t *bgTiles[4] = {
            _tiles[_bg[(topLeftY << 4) + topLeftX]],
            _tiles[_bg[(topLeftY << 4) + topLeftX + 1]],
            _tiles[_bg[((topLeftY + 1) << 4) + topLeftX]],
            _tiles[_bg[((topLeftY + 1) << 4) + topLeftX + 1]]
        };
        _display.drawOffsetTile(
            _sprs[i].x, _sprs[i].y, _tiles[_sprs[i].image], bgTiles
        );
    }
}

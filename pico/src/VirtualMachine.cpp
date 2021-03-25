#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/irq.h>
#include <Rom.hpp>
#include <Display.hpp>
#include <Controls.hpp>
#include <VirtualMachine.hpp>

using namespace gamecard;

const Display *VirtualMachine::_disp = nullptr;
Sprite VirtualMachine::_sprs[VM_MAX_SPRITES];
Image VirtualMachine::_tiles[VM_MAX_TILES];
uint8_t VirtualMachine::_bg[VM_MAP_SIZE];

VirtualMachine::VirtualMachine(
        const Display &disp, const Controller &cont, const RomChip &rom) :
        _cont(cont), _rom(rom) {
    _disp = &disp;
    _disp->fill(true);          // Clear display
    
    // Initialize memory
    for(int i =0 ; i < VM_MAP_SIZE; i++) {
        _bg[i] = 0;
    }
    for(int i = 0; i < VM_MAX_TILES; i++) {
        for(int j = 0; j < 8; j++) {
            _tiles[i][j] = 0;
        }
    }
    for(int i = 0; i < VM_MAX_SPRITES; i++) {
        _sprs[i] = (Sprite) { 0, 0, 0 };
    }
    for(int i = 0; i < VM_NUM_REGS; i++) {
        _regs[i] = 0;
    }
    pc = 0;
    
    // Set up display multicore
    multicore_launch_core1(_displayThread);
    
    // Main thread:
    while(1) {
        //printf("Main thread!\n");
    }
}

void VirtualMachine::_displayThread() {
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, _displayThreadIrq);
    irq_set_enabled(SIO_IRQ_PROC1, true);
    
    while(1) {
        tight_loop_contents();
    }
}

// When data is pushed to this core, handle it on this core
void VirtualMachine::_displayThreadIrq() {
    while(multicore_fifo_rvalid()) {
        const auto data = multicore_fifo_pop_blocking();
        switch(data) {
            case 0:             // update everything
                _updateSprites();
                _updateMap();
                break;
            
            case 1:             // update just sprites
                _updateSprites();
                break;
            
            case 2:             // update just map
                _updateMap();
                break;
            
            case 3: {
                while(!multicore_fifo_rvalid());
                const auto index = multicore_fifo_pop_blocking();
                _clearSprite(index);
            } break;
        }
        break;
    }
}

void VirtualMachine::_clearSprite(uint8_t index) {
    uint8_t row = _sprs[index].y >> 3;
    uint8_t col = _sprs[index].x >> 3;
    _disp->putTile(_tiles[_bg[(row << 4) + col]], col, row);
    _disp->putTile(_tiles[_bg[(row << 4) + col + 1]], col + 1, row);
    _disp->putTile(_tiles[_bg[((row + 1) << 4) + col]], col, row + 1);
    _disp->putTile(_tiles[_bg[((row + 1) << 4) + col + 1]], col + 1, row + 1);
}

void VirtualMachine::_updateSprites() {
    for(int i = 0; i < VM_MAX_SPRITES; i++) {
        if(_sprs[i].imageIndex != 0) {
            uint8_t row = _sprs[i].y >> 3;
            uint8_t col = _sprs[i].x >> 3;
            
            uint8_t bgTiles[4][8];
            for(int j = 0; j < 8; j++) {
                bgTiles[0][j] = _tiles[_bg[(row << 4) + col]][j];
                bgTiles[1][j] = _tiles[_bg[(row << 4) + col + 1]][j];
                bgTiles[2][j] = _tiles[_bg[((row + 1) << 4) + col]][j];
                bgTiles[3][j] = _tiles[_bg[((row + 1) << 4) + col + 1]][j];
            }
            _disp->putOffsetTile(
                _sprs[i].x, _sprs[i].y, _tiles[_sprs[i].imageIndex], bgTiles
            );
        }
    }
}

void VirtualMachine::_updateMap() {
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 16; col++) {
            _disp->putTile(_tiles[_bg[(row << 4) + col]], col, row);
        }
    }
}

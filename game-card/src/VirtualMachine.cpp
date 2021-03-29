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
const Controller *VirtualMachine::_cont = nullptr;

Sprite VirtualMachine::_sprs[VM_MAX_SPRITES];
Image VirtualMachine::_tiles[VM_MAX_TILES];
uint8_t VirtualMachine::_bg[VM_MAP_SIZE];
int32_t VirtualMachine::_regs[VM_NUM_REGS];

void VirtualMachine::_displayThread() {
    printf("Hello from second core!\n");
    
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, _displayThreadIrq);
    irq_set_enabled(SIO_IRQ_PROC1, true);
    
    printf("Second thread done initializing!\n");
    
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
                while(!multicore_fifo_wready());
                multicore_fifo_push_blocking(1);
                
                while(!multicore_fifo_rvalid());
                const auto index = multicore_fifo_pop_blocking();
                
                const auto row = _sprs[index].y >> 3;
                const auto col = _sprs[index].x >> 3;
                _disp->putTile(_tiles[_bg[(row << 4) + col]], col, row);
                _disp->putTile(_tiles[_bg[(row << 4) + col + 1]], col + 1, row);
                _disp->putTile(
                    _tiles[_bg[((row + 1) << 4) + col]], col, row + 1
                );
                _disp->putTile(
                    _tiles[_bg[((row + 1) << 4) + col + 1]], col + 1, row + 1
                );
            } break;
            
            case 4:             // End
                _disp->fill(true);
                return;
            
            case 5:             // Update controller reg
                const auto up =
                    _cont->isPressed(ControllerInput::Up) ? (1 << 5) : 0;
                const auto down =
                    _cont->isPressed(ControllerInput::Down) ? (1 << 4) : 0;
                const auto left =
                    _cont->isPressed(ControllerInput::Left) ? (1 << 3) : 0;
                const auto right =
                    _cont->isPressed(ControllerInput::Right) ? (1 << 2) : 0;
                const auto a =
                    _cont->isPressed(ControllerInput::A) ? (1 << 1) : 0;
                const auto b = _cont->isPressed(ControllerInput::B) ? 1 : 0;
                _regs[0] = up + down + left + right + a + b;
                break;
        }
        break;
    }
}

void VirtualMachine::_clearSprite(uint8_t index) {
    // Send clear signal
    while(!multicore_fifo_wready());
    multicore_fifo_push_blocking(3);
    
    // Wait for got
    while(!multicore_fifo_rvalid());
    multicore_fifo_pop_blocking();
    
    // Send index
    while(!multicore_fifo_wready());
    multicore_fifo_push_blocking(index);
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

VirtualMachine::VirtualMachine(
        const Display &disp, const Controller &cont, const RomChip &rom) {
    _disp = &disp;
    _disp->fill(true);          // Clear display
    
    _cont = &cont;
    
    auto _cmpReg = CompareState::Equal;
    uint8_t _rom[rom.size()];
    uint64_t pc;
    
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
    printf("Initializing display thread\n");
    multicore_launch_core1(_displayThread);
    
    // Load the program into memory
    printf("Reading program size: ");
    //uint8_t size[8] = { 0, 0, 0, 0, 0, 0, 0, 0x43 };
    //rom.write(0, size, 8);
    for(int i = 0; i < 8; i++) {
        rom.read(i, _rom + i, 1);
        printf("%llx ", static_cast<uint64_t>(_rom[i]));
    }
    printf(" -> ");
    uint64_t progSize =
        (static_cast<uint64_t>(_rom[0]) << 56)
        + (static_cast<uint64_t>(_rom[1]) << 48)
        + (static_cast<uint64_t>(_rom[2]) << 40)
        + (static_cast<uint64_t>(_rom[3]) << 32)
        + (static_cast<uint64_t>(_rom[4]) << 24)
        + (static_cast<uint64_t>(_rom[5]) << 16)
        + (static_cast<uint64_t>(_rom[6]) << 8)
        + static_cast<uint64_t>(_rom[7]);
    printf("%llu\n", progSize);
    
    printf("Loading cartridge into ram.\n");
    for(uint64_t i = 8; i < progSize; i++) {
        rom.read(i, _rom + i, 1);
        printf("[%llu]=%x ", i, _rom[i]);
    }
    printf("\n");
    
    pc += 8;
        
    // Main thread:
    while(pc < progSize) {
        printf("Command: rom[%llu] - %x\n", pc, _rom[pc]);
        switch(_rom[pc++]) {
            /*
             * Set entire sprite
             */
            // _sprs[<literal>] = { <literal>, <literal>, <literal> }
            case 0x00:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]] = (Sprite) {
                    _rom[pc + 1], _rom[pc + 2], _rom[pc + 3]
                };
                pc += 4;
                break;
            // _sprs[<register>] = { <literal>, <literal>, <literal> }
            case 0x01:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])] = (Sprite) {
                    _rom[pc + 1], _rom[pc + 2], _rom[pc + 3]
                };
                pc += 4;
                break;
            // _sprs[<literal>] = { <register>, <literal>, <literal> }
            case 0x02:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]] = (Sprite) {
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]),
                    _rom[pc + 2],
                    _rom[pc + 3]
                };
                pc += 4;
                break;
            // _sprs[<reg>] = { <reg>, <literal>, <literal> }
            case 0x03:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])] = (Sprite) {
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]),
                    _rom[pc + 2],
                    _rom[pc + 3]
                };
                pc += 4;
                break;
            // _sprs[<literal>] = { <literal>, <reg>, <literal> }
            case 0x04:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]] = (Sprite) {
                    _rom[pc + 1],
                    static_cast<uint8_t>(_regs[_rom[pc + 2]]),
                    _rom[pc + 3]
                };
                break;
            // _sprs[<reg>] = { <literal>, <reg>, <literal> }
            case 0x05:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])] = (Sprite) {
                    _rom[pc + 1],
                    static_cast<uint8_t>(_regs[_rom[pc + 2]]),
                    _rom[pc + 3]
                };
                pc += 4;
                break;
            // _sprs[<literal>] = { <reg>, <reg>, <literal> }
            case 0x06:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]] = (Sprite) {
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]),
                    static_cast<uint8_t>(_regs[_rom[pc + 2]]),
                    _rom[pc + 3]
                };
                pc += 4;
                break;
            // _sprs[<reg>] = { <reg>, <reg>, <literal> }
            case 0x07:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])] = (Sprite) {
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]),
                    static_cast<uint8_t>(_regs[_rom[pc + 2]]),
                    _rom[pc + 3]
                };
                pc += 4;
                break;
            // _sprs[<literal>] = { <literal>, <literal>, <reg> }
            case 0x08:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]] = (Sprite) {
                    _rom[pc + 1],
                    _rom[pc + 2],
                    static_cast<uint8_t>(_regs[_rom[pc + 3]])
                };
                pc += 4;
                break;
            // _sprs[<register>] = { <literal>, <literal>, <reg> }
            case 0x09:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])] = (Sprite) {
                    _rom[pc + 1],
                    _rom[pc + 2],
                    static_cast<uint8_t>(_regs[_rom[pc + 3]])
                };
                pc += 4;
                break;
            // _sprs[<literal>] = { <register>, <literal>, <reg> }
            case 0x0A:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]] = (Sprite) {
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]),
                    _rom[pc + 2],
                    static_cast<uint8_t>(_regs[_rom[pc + 3]])
                };
                break;
            // _sprs[<reg>] = { <reg>, <literal>, <reg> }
            case 0x0B:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])] = (Sprite) {
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]),
                    _rom[pc + 2],
                    static_cast<uint8_t>(_regs[_rom[pc + 3]])
                };
                pc += 4;
                break;
            // _sprs[<literal>] = { <literal>, <reg>, <reg> }
            case 0x0C:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]] = (Sprite) {
                    _rom[pc + 1],
                    static_cast<uint8_t>(_regs[_rom[pc + 2]]),
                    static_cast<uint8_t>(_regs[_rom[pc + 3]])
                };
                pc += 4;
                break;
            // _sprs[<reg>] = { <literal>, <reg>, <reg> }
            case 0x0D:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])] = (Sprite) {
                    _rom[pc + 1],
                    static_cast<uint8_t>(_regs[_rom[pc + 2]]),
                    static_cast<uint8_t>(_regs[_rom[pc + 3]])
                };
                pc += 4;
                break;
            // _sprs[<literal>] = { <reg>, <reg>, <reg> }
            case 0x0E:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]] = (Sprite) {
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]),
                    static_cast<uint8_t>(_regs[_rom[pc + 2]]),
                    static_cast<uint8_t>(_regs[_rom[pc + 3]])
                };
                pc += 4;
                break;
            // _sprs[<reg>] = { <reg>, <reg>, <reg> }
            case 0x0F:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])] = (Sprite) {
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]),
                    static_cast<uint8_t>(_regs[_rom[pc + 2]]),
                    static_cast<uint8_t>(_regs[_rom[pc + 3]])
                };
                pc += 4;
                break;
            
            /*
             * Set sprite x
             */
            // _sprs[<lit>].x = <lit>
            case 0x10:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].x = _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].x = <lit>
            case 0x11:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x = _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].x = <reg>
            case 0x12:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x =
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].x = <reg>
            case 0x13:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x =
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].x += <lit>
            case 0x14:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].x += _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].x += <lit>
            case 0x15:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x += _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].x += <reg>
            case 0x16:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x +=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].x += <reg>
            case 0x17:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x +=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].x -= <lit>
            case 0x18:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].x -= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].x -= <lit>
            case 0x19:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x -= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].x -= <reg>
            case 0x1A:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x -=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].x -= <reg>
            case 0x1B:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x -=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].x *= <lit>
            case 0x1C:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].x *= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].x *= <lit>
            case 0x1D:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x *= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].x *= <reg>
            case 0x1E:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x *=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].x *= <reg>
            case 0x1F:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x *=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].x /= <lit>
            case 0x20:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].x /= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].x /= <lit>
            case 0x21:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x /= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].x /= <reg>
            case 0x23:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x /=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].x /= <reg>
            case 0x24:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x /=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].x >>= <lit>
            case 0x25:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].x >>= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].x >>= <lit>
            case 0x26:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x >>= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].x >>= <reg>
            case 0x27:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x >>=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].x >>= <reg>
            case 0x28:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x >>=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].x <<= <lit>
            case 0x29:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].x <<= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].x <<= <lit>
            case 0x2A:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x <<= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].x <<= <reg>
            case 0x2B:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x <<=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].x <<= <reg>
            case 0x2C:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].x <<=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            
            /*
             * Set sprite y
             */
            // _sprs[<lit>].y = <lit>
            case 0x2D:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].y = _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].y = <lit>
            case 0x2E:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y = _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].y = <reg>
            case 0x2F:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y =
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].y = <reg>
            case 0x30:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y =
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].y += <lit>
            case 0x31:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].y += _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].y += <lit>
            case 0x32:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y += _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].y += <reg>
            case 0x33:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y +=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].y += <reg>
            case 0x34:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y +=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].y -= <lit>
            case 0x35:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].y -= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].y -= <lit>
            case 0x36:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y -= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].y -= <reg>
            case 0x37:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y -=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].y -= <reg>
            case 0x38:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y -=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].y *= <lit>
            case 0x39:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].y *= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].y *= <lit>
            case 0x3A:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y *= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].y *= <reg>
            case 0x3B:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y *=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].y *= <reg>
            case 0x3C:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y *=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].y /= <lit>
            case 0x3D:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].y /= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].y /= <lit>
            case 0x3E:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y /= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].y /= <reg>
            case 0x3F:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y /=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].y /= <reg>
            case 0x40:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y /=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].y >>= <lit>
            case 0x41:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].y >>= _rom[pc];
                pc += 2;
                break;
            // _sprs[<reg>].y >>= <lit>
            case 0x42:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y >>= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].y >>= <reg>
            case 0x43:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y >>=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].y >>= <reg>
            case 0x44:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y >>=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<lit>].y <<= <lit>
            case 0x45:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].y <<= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].y <<= <lit>
            case 0x46:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y <<= _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].y <<= <reg>
            case 0x47:
                _clearSprite(_rom[pc]);
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y <<=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].y <<= <reg>
            case 0x48:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y <<=
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            
            /*
             * Sprite image index
             */
            // _sprs[<lit>].imageIndex = <lit>
            case 0x49:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].imageIndex = _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<reg>].imageIndex = <lit>
            case 0x4A:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].imageIndex =
                    _rom[pc + 1];
                pc += 2;
                break;
            // _sprs[<lit>].imageIndex = <reg>
            case 0x4B:
                _clearSprite(_rom[pc]);
                _sprs[_rom[pc]].imageIndex =
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _sprs[<reg>].imageIndex = <reg>
            case 0x4C:
                _clearSprite(static_cast<uint8_t>(_regs[_rom[pc]]));
                _sprs[static_cast<uint8_t>(_regs[_rom[pc]])].y <<=
                    static_cast<uint8_t>(_regs[_rom[pc]]);
                pc += 2;
                break;
            
            /*
             * Copy tile
             */
            case 0x4D:
                for(int i = 0; i < 8; i++) {
                    _tiles[_rom[pc]][i] = _rom[pc + 1 + i];
                }
                pc += 9;
                break;
            
            /*
             * Set background
             */
            // _bg[<lit>] = <lit>
            case 0x4E:
                _bg[_rom[pc]] = _rom[pc + 1];
                pc += 2;
                break;
            // _bg[<reg>] = <lit>
            case 0x4F:
                _bg[static_cast<uint8_t>(_regs[_rom[pc]])] = _rom[pc + 1];
                pc += 2;
                break;
            // _bg[<lit>] = <reg>
            case 0x50:
                _bg[_rom[pc]] = static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            // _bg[<reg>] = <reg>
            case 0x51:
                _bg[static_cast<uint8_t>(_regs[_rom[pc]])] =
                    static_cast<uint8_t>(_regs[_rom[pc + 1]]);
                pc += 2;
                break;
            
            /*
             * Update graphics
             */
            case 0x52:
                while(!multicore_fifo_wready());
                multicore_fifo_push_blocking(0);
                pc++;
                break;
            case 0x53:
                while(!multicore_fifo_wready());
                multicore_fifo_push_blocking(1);
                pc++;
                break;
            case 0x54:
                while(!multicore_fifo_wready());
                multicore_fifo_push_blocking(2);
                pc++;
                break;
            
            /*
             * Set register
             */
            // _regs[<lit>] = <lit>
            case 0x55:
                _regs[_rom[pc]] =
                    (static_cast<int32_t>(_rom[pc + 1]) << 24)
                    + (static_cast<int32_t>(_rom[pc + 2]) << 16)
                    + (static_cast<int32_t>(_rom[pc + 3]) << 8)
                    + static_cast<int32_t>(_rom[pc + 4]);
                pc += 5;
                break;
            // _regs[<lit>] = _regs[<lit>]
            case 0x56:
                _regs[_rom[pc]] = _regs[_rom[pc + 1]];
                pc += 2;
                break;
            // _regs[<lit>] += <lit>
            case 0x57:
                _regs[_rom[pc]] +=
                    (static_cast<int32_t>(_rom[pc + 1]) << 24)
                    + (static_cast<int32_t>(_rom[pc + 2]) << 16)
                    + (static_cast<int32_t>(_rom[pc + 3]) << 8)
                    + static_cast<int32_t>(_rom[pc + 4]);
                /*printf(
                    "_regs[%d] += { %x, %x, %x, %x } -> %ld\n",
                    _rom[pc],
                    _rom[pc + 1], _rom[pc + 2], _rom[pc + 3], _rom[pc + 4],
                    _regs[_rom[pc]]
                );*/
                pc += 5;
                break;
            // _regs[<lit>] += _regs[<lit>]
            case 0x58:
                _regs[_rom[pc]] += _regs[_rom[pc + 1]];
                pc += 2;
                break;
            // _regs[<lit>] -= <lit>
            case 0x59:
                _regs[_rom[pc]] -=
                    (static_cast<int32_t>(_rom[pc + 1]) << 24)
                    + (static_cast<int32_t>(_rom[pc + 2]) << 16)
                    + (static_cast<int32_t>(_rom[pc + 3]) << 8)
                    + static_cast<int32_t>(_rom[pc + 4]);
                pc += 5;
                break;
            // _regs[<lit>] -= _regs[<lit>]
            case 0x5A:
                _regs[_rom[pc]] -= _regs[_rom[pc + 1]];
                pc += 2;
                break;
            // _regs[<lit>] *= <lit>
            case 0x5B:
                _regs[_rom[pc]] *=
                    (static_cast<int32_t>(_rom[pc + 1]) << 24)
                    + (static_cast<int32_t>(_rom[pc + 2]) << 16)
                    + (static_cast<int32_t>(_rom[pc + 3]) << 8)
                    + static_cast<int32_t>(_rom[pc + 4]);
                pc += 5;
                break;
            // _regs[<lit>] *= _regs[<lit>]
            case 0x5C:
                _regs[_rom[pc]] *= _regs[_rom[pc + 1]];
                pc += 2;
                break;
            // _regs[<lit>] /= <lit>
            case 0x5D:
                _regs[_rom[pc]] /=
                    (static_cast<int32_t>(_rom[pc + 1]) << 24)
                    + (static_cast<int32_t>(_rom[pc + 2]) << 16)
                    + (static_cast<int32_t>(_rom[pc + 3]) << 8)
                    + static_cast<int32_t>(_rom[pc + 4]);
                pc += 5;
                break;
            // _regs[<lit>] /= _regs[<lit>]
            case 0x5E:
                _regs[_rom[pc]] /= _regs[_rom[pc + 1]];
                pc += 2;
                break;
            // _regs[<lit>] >>= <lit>
            case 0x5F:
                _regs[_rom[pc]] >>=
                    (static_cast<int32_t>(_rom[pc + 1]) << 24)
                    + (static_cast<int32_t>(_rom[pc + 2]) << 16)
                    + (static_cast<int32_t>(_rom[pc + 3]) << 8)
                    + static_cast<int32_t>(_rom[pc + 4]);
                pc += 5;
                break;
            // _regs[<lit>] >>= _regs[<lit>]
            case 0x60:
                _regs[_rom[pc]] >>= _regs[_rom[pc + 1]];
                pc += 2;
                break;
            // _regs[<lit>] <<= <lit>
            case 0x61:
                _regs[_rom[pc]] <<=
                    (static_cast<int32_t>(_rom[pc + 1]) << 24)
                    + (static_cast<int32_t>(_rom[pc + 2]) << 16)
                    + (static_cast<int32_t>(_rom[pc + 3]) << 8)
                    + static_cast<int32_t>(_rom[pc + 4]);
                pc += 5;
                break;
            // _regs[<lit>] <<= _regs[<lit>]
            case 0x62:
                _regs[_rom[pc]] <<= _regs[_rom[pc + 1]];
                pc += 2;
                break;
            
            /*
             * Compare
             */
            case 0x63:
                _cmpReg =
                    (_regs[_rom[pc]] == _regs[_rom[pc + 1]]) ?
                        CompareState::Equal :
                        ((_regs[_rom[pc]] < _regs[_rom[pc + 1]]) ?
                            CompareState::LessThan :
                            CompareState::GreaterThan);
                pc += 2;
                break;
            
            /*
             * Jump
             */
            // jump
            case 0x64:
                pc = 
                    (static_cast<uint64_t>(_rom[pc]) << 56)
                    + (static_cast<uint64_t>(_rom[pc + 1]) << 48)
                    + (static_cast<uint64_t>(_rom[pc + 2]) << 40)
                    + (static_cast<uint64_t>(_rom[pc + 3]) << 32)
                    + (static_cast<uint64_t>(_rom[pc + 4]) << 24)
                    + (static_cast<uint64_t>(_rom[pc + 5]) << 16)
                    + (static_cast<uint64_t>(_rom[pc + 6]) << 8)
                    + static_cast<uint64_t>(_rom[pc + 7]);
                break;
            // jump if ==
            case 0x65:
                if(_cmpReg == CompareState::Equal) {
                    pc = 
                        (static_cast<uint64_t>(_rom[pc]) << 56)
                        + (static_cast<uint64_t>(_rom[pc + 1]) << 48)
                        + (static_cast<uint64_t>(_rom[pc + 2]) << 40)
                        + (static_cast<uint64_t>(_rom[pc + 3]) << 32)
                        + (static_cast<uint64_t>(_rom[pc + 4]) << 24)
                        + (static_cast<uint64_t>(_rom[pc + 5]) << 16)
                        + (static_cast<uint64_t>(_rom[pc + 6]) << 8)
                        + static_cast<uint64_t>(_rom[pc + 7]);
                } else {
                    pc += 8;
                }
                break;
            // jump if <
            case 0x66:
                if(_cmpReg == CompareState::LessThan) {
                    pc = 
                        (static_cast<uint64_t>(_rom[pc]) << 56)
                        + (static_cast<uint64_t>(_rom[pc + 1]) << 48)
                        + (static_cast<uint64_t>(_rom[pc + 2]) << 40)
                        + (static_cast<uint64_t>(_rom[pc + 3]) << 32)
                        + (static_cast<uint64_t>(_rom[pc + 4]) << 24)
                        + (static_cast<uint64_t>(_rom[pc + 5]) << 16)
                        + (static_cast<uint64_t>(_rom[pc + 6]) << 8)
                        + static_cast<uint64_t>(_rom[pc + 7]);
                } else {
                    pc += 8;
                }
                break;
            // jump if >
            case 0x67:
                if(_cmpReg == CompareState::GreaterThan) {
                    pc = 
                        (static_cast<uint64_t>(_rom[pc]) << 56)
                        + (static_cast<uint64_t>(_rom[pc + 1]) << 48)
                        + (static_cast<uint64_t>(_rom[pc + 2]) << 40)
                        + (static_cast<uint64_t>(_rom[pc + 3]) << 32)
                        + (static_cast<uint64_t>(_rom[pc + 4]) << 24)
                        + (static_cast<uint64_t>(_rom[pc + 5]) << 16)
                        + (static_cast<uint64_t>(_rom[pc + 6]) << 8)
                        + static_cast<uint64_t>(_rom[pc + 7]);
                } else {
                    pc += 8;
                }
                break;
            // jump if <=
            case 0x68:
                if(_cmpReg == CompareState::Equal
                        || _cmpReg == CompareState::LessThan) {
                    pc = 
                        (static_cast<uint64_t>(_rom[pc]) << 56)
                        + (static_cast<uint64_t>(_rom[pc + 1]) << 48)
                        + (static_cast<uint64_t>(_rom[pc + 2]) << 40)
                        + (static_cast<uint64_t>(_rom[pc + 3]) << 32)
                        + (static_cast<uint64_t>(_rom[pc + 4]) << 24)
                        + (static_cast<uint64_t>(_rom[pc + 5]) << 16)
                        + (static_cast<uint64_t>(_rom[pc + 6]) << 8)
                        + static_cast<uint64_t>(_rom[pc + 7]);
                } else {
                    pc += 8;
                }
                break;
            // jump if >=
            case 0x69:
                if(_cmpReg == CompareState::Equal
                        || _cmpReg == CompareState::GreaterThan) {
                    pc = 
                        (static_cast<uint64_t>(_rom[pc]) << 56)
                        + (static_cast<uint64_t>(_rom[pc + 1]) << 48)
                        + (static_cast<uint64_t>(_rom[pc + 2]) << 40)
                        + (static_cast<uint64_t>(_rom[pc + 3]) << 32)
                        + (static_cast<uint64_t>(_rom[pc + 4]) << 24)
                        + (static_cast<uint64_t>(_rom[pc + 5]) << 16)
                        + (static_cast<uint64_t>(_rom[pc + 6]) << 8)
                        + static_cast<uint64_t>(_rom[pc + 7]);
                } else {
                    pc += 8;
                }
                break;
            // jump if !=
            case 0x6A:
                if(_cmpReg != CompareState::Equal) {
                    pc = 
                        (static_cast<uint64_t>(_rom[pc]) << 56)
                        + (static_cast<uint64_t>(_rom[pc + 1]) << 48)
                        + (static_cast<uint64_t>(_rom[pc + 2]) << 40)
                        + (static_cast<uint64_t>(_rom[pc + 3]) << 32)
                        + (static_cast<uint64_t>(_rom[pc + 4]) << 24)
                        + (static_cast<uint64_t>(_rom[pc + 5]) << 16)
                        + (static_cast<uint64_t>(_rom[pc + 6]) << 8)
                        + static_cast<uint64_t>(_rom[pc + 7]);
                } else {
                    pc += 8;
                }
                break;
            
            /*
             * Update controller input into reg 0
             */
            case 0x6B:
                while(!multicore_fifo_wready());
                multicore_fifo_push_blocking(5);
                break;
            
            /*
             * Delay in ms
             */
            // sleep_ms(<lit>)
            case 0x6C:
                sleep_ms(
                    (static_cast<uint32_t>(_rom[pc]) << 24)
                    + (static_cast<uint32_t>(_rom[pc + 1]) << 16)
                    + (static_cast<uint32_t>(_rom[pc + 2]) << 8)
                    + static_cast<uint32_t>(_rom[pc + 3])
                );
                pc += 4;
                break;
            // sleep_ms(<reg>)
            case 0x6D:
                sleep_ms(_regs[_rom[pc]]);
                pc++;
                break;
        }
    }
    
    while(!multicore_fifo_wready());
    multicore_fifo_push_blocking(4);
    return;
}

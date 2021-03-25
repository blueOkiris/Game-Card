/*
 * This file is the brains of the system
 * It manages memory and includes the interpreter
 * as well as puts all the hardware stuff together
 */
#pragma once

#include <memory>
#include <Display.hpp>
#include <Controls.hpp>
#include <Rom.hpp>

#define VM_MAP_SIZE         128             // 128/8 * 64/8 = 16 * 8 = 128
#define VM_MAX_SPRITES      32
#define VM_MAX_TILES        64
#define VM_NUM_REGS         32
#define VM_CMD_LEN          10
#define VM_INPUT_REG        0
#define VM_PROG_SIZE        (128 * 1024)    // 1 Mb = 1024 Kb = 128 KB

namespace gamecard {
    // Structs for storing image data for efficient drawing
    struct Sprite {
        uint8_t x, y;
        uint8_t imageIndex;
    };
    typedef uint8_t Image[8];
    
    // Value for the compare register
    enum class CompareState {
        Equal, LessThan, GreaterThan
    };
    
    class VirtualMachine {
        private:
            // Multicore
            static const Display *_disp;
            static Sprite _sprs[VM_MAX_SPRITES];
            static Image _tiles[VM_MAX_TILES];
            static uint8_t _bg[VM_MAP_SIZE];
            
            static void _displayThreadIrq();
            static void _displayThread();
            
            static void _updateSprites();
            static void _updateMap();
            static void _clearSprite(uint8_t index);
            
            // Hardware
            const Controller &_cont;
            
            // Memory
            uint8_t _rom[VM_PROG_SIZE];
            int32_t _regs[VM_NUM_REGS];
            CompareState _cmpReg;
            uint64_t pc;
            
        public:
            VirtualMachine(
                const Display &disp,
                const Controller &cont,
                const RomChip &rom
            );
    };
}

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

#define VM_MAP_SIZE         128     // 128/8 * 64/8 = 16 * 8 = 128
#define VM_MAX_SPRITES      32
#define VM_MAX_TILES        64
#define VM_NUM_REGS         32
#define VM_CMD_LEN          10
#define VM_INPUT_REG        0

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
            static void _displayThreadIrq();
            static void _displayThread();
            
            // Hardware
            const std::shared_ptr<Display> _disp;
            const std::shared_ptr<Controller> _cont;
            const std::shared_ptr<RomChip> _rom;
            
            // Memory
            Image _tiles[VM_MAX_TILES];
            uint8_t _bg[VM_MAP_SIZE];
            Sprite _sprs[VM_MAX_SPRITES];
            int32_t _regs[VM_NUM_REGS];
            CompareState _cmpReg;
            uint64_t pc;
            
        public:
            VirtualMachine(
                const std::shared_ptr<Display> &disp,
                const std::shared_ptr<Controller> &cont,
                const std::shared_ptr<RomChip> &rom
            );
    };
}

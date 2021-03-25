#include <stdio.h>
#include <memory>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/irq.h>
#include <Rom.hpp>
#include <Display.hpp>
#include <Controls.hpp>
#include <VirtualMachine.hpp>

using namespace gamecard;

VirtualMachine::VirtualMachine(
        const std::shared_ptr<Display> &disp,
        const std::shared_ptr<Controller> &cont,
        const std::shared_ptr<RomChip> &rom) :
        _disp(disp), _cont(cont), _rom(rom) {
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
    }
}

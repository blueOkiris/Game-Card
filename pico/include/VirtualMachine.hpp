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

namespace gamecard {
    class VirtualMachine {
        private:
            const std::shared_ptr<Display> _disp;
            const std::shared_ptr<Controller> _cont;
            const std::shared_ptr<RomChip> _rom;
            
        public:
            VirtualMachine(
                const std::shared_ptr<Display> &disp,
                const std::shared_ptr<Controller> &cont,
                const std::shared_ptr<RomChip> &rom
            );
    };
}

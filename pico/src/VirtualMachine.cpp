#include <VirtualMachine.hpp>

using namespace gamecard;

VirtualMachine::VirtualMachine(
        const std::shared_ptr<Display> &disp,
        const std::shared_ptr<Controller> &cont,
        const std::shared_ptr<RomChip> &rom) :
        _disp(disp), _cont(cont), _rom(rom) {
}

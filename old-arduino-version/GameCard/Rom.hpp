#ifndef _ROM_HPP_
#define _ROM_HPP_

#include "Spi.hpp"
#include "Device.hpp"

namespace gamecard {
    // We only care about READING here
    
    #define ROM_CMD_READ    0x03
    #define ROM_CS_BMASK    0x04    // Pin 10 -> 1 << (10 - 8) = 0x04 for port b
    
    // 512 kb = 64kB
    class Eeprom25LC512 {
        private:
            const SpiBus _spi = SpiBus();
            
        public:
            void init() const;
            void instruction(
                uint16_t addr, uint8_t buffer[VM_CMD_LEN]
            ) const;
    };
}

#endif
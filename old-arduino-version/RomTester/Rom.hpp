#ifndef _ROM_HPP_
#define _ROM_HPP_

#include "Spi.hpp"

namespace tester {
    #define ROM_CMD_READ    0x03
    #define ROM_CMD_WRITE   0x02
    #define ROM_CMD_WREN    0x06
    #define ROM_SPI_CS      10
    
    // 512 kb = 64kB
    class Eeprom25LC512 {
        private:
            const SpiBus _spi = SpiBus();
            
        public:
            void init() const;
            uint8_t read(uint16_t addr) const;
            void write(uint16_t page, uint8_t data[128]) const;
    };
}

#endif
#ifndef _ROM_HPP_
#define _ROM_HPP_

#include "Spi.hpp"

namespace gamecard {
    #define M23A1024_READ   0x03
    #define M23A1024_WRITE  0x02
    #define M23A1024_EDIO   0x3B
    #define M23A1024_EQIO   0x28
    #define M23A1024_RSTIO  0xFF
    #define M23A1024_RDMR   0x05
    #define M23A1024_WRMR   0x01

    #define M23A1024_REG_BM 0x00
    
    #define M23A1024_SPI_CS 10
    
    class M23a1024 {
        private:
            const SpiBus _spi;
            void _writeReg(const uint8_t reg) const;
            
        public:
            M23a1024();
            void write(
                const uint32_t addr, const uint8_t *buff, const int len
            ) const;
    };
}

#endif
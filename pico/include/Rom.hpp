/*
 * This file is for talking to the ROM devices
 * Specifically the 23A1024
 */
#pragma once

#define PIN_MISO        12
#define PIN_CS          13
#define PIN_SCK         14
#define PIN_MOSI        11

#define SPI_PORT        spi1
#define SPI_SPEED       20000000

#define M23A1024_READ   0x03
#define M23A1024_WRITE  0x02
#define M23A1024_EDIO   0x3B
#define M23A1024_EQIO   0x28
#define M23A1024_RSTIO  0xFF
#define M23A1024_RDMR   0x05
#define M23A1024_WRMR   0x01

#define M23A1024_REG_BM 0x00

namespace gamecard {
    class M23a1024 {
        private:
            void _writeReg(const uint8_t reg);
            
        public:
            M23a1024();
            void write(
                const uint32_t addr, const uint8_t *buff, const int len
            ) const;
            void read(
                const uint32_t addr, uint8_t *buff, const int len
            ) const;
    };
};

/*
 * This file is for talking to the ROM devices
 * Specifically the 23A1024
 * 
 * If I decide to go the NFC route, the two classes CANNOT be used
 * with each other
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

#define M25LC512_READ   0x03
#define M25LC512_WRITE  0x02
#define M25LC512_WREN   0x06
#define M25LC512_WRDI   0x04
#define M25LC512_WRSR   0x01
#define M25LC512_RDSR   0x05

namespace gamecard {
    union Instruction {
        uint32_t val;
        struct {
            uint8_t lsb;
            uint8_t nlsb;
            uint8_t pmsb;
            uint8_t msb;
        };
    };
    
    class RomChip {
        public:
            virtual void write(
                const uint32_t addr, const uint8_t *buff, const int len
            ) const = 0;
            virtual void read(
                const uint32_t addr, uint8_t *buff, const int len
            ) const = 0;
            virtual uint64_t size() const = 0;
    };
    
    // SRAM implementation of game ROM (Volatile!)
    class M23a1024 : public RomChip {
        private:
            void _writeReg(const uint8_t reg) const;
            
        public:
            M23a1024();
            void write(
                const uint32_t addr, const uint8_t *buff, const int len
            ) const override;
            void read(
                const uint32_t addr, uint8_t *buff, const int len
            ) const override;
            uint64_t size() const override;
    };
    
    // Eeprom implementation of game ROM (Similar to above, but non-volatile)
    class M25lc512 : public RomChip {
        private:
            void _wren() const;
            void _wrdi() const;
        
        public:
            M25lc512();
            void write(
                const uint32_t addr, const uint8_t *buff, const int len
            ) const override;
            void read(
                const uint32_t addr, uint8_t *buff, const int len
            ) const override;
            uint64_t size() const override;
    };
};

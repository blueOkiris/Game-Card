#ifndef _ROM_HPP_
#define _ROM_HPP_

namespace gamecard {
    /*
     * Instruction Set
     * 
     * | Name    | Data    | Description                         |
     * -----------------------------------------------------------
     * | READ    | 0x03    | Read data from mem at selected addr |
     * | WRITE   | 0x02    | Write data to mem at selected addr  |
     * | WREN    | 0x06    | Set write enable                    |
     * | WRDI    | 0x04    | Disable write operations            |
     * | RDSR    | 0x05    | Read status register                |
     * | WRSR    | 0x01    | Write to status register            |
     * | PE      | 0x42    | Clear out an entire page            |
     * | SE      | 0xD8    | Clear out an entire sector          |
     * | CE      | 0xC7    | Erase ENTIRE memory                 |
     * | RDID    | 0xAB    | Wake up                             |
     * | DPD     | 0xA9    | Sleep                               |
     * 
     * We only care about reading from the address though
     */
    #define ROM_CMD_READ    0x03
    #define ROM_CMD_WRITE   0x02
    #define ROM_SPI_CS      10
    
    // 512 kb = 64kB
    class Eeprom25LC512 {
        public:
            void init() const;
            uint8_t read(uint16_t addr) const;
            void write(uint16_t addr, uint8_t byte) const;
    };
}

#endif
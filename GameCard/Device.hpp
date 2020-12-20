#ifndef _DEVICE_HPP_
#define _DEVICE_HPP_

// Oled driver commands
#define SSD_CMD_SET_CONTRAST    0x81
#define SSD_CMD_OUTPUT_IS_RAM   0xA4
#define SSD_CMD_OUTPUT_NOT_RAM  0xA5
#define SSD_CMD_DISP_NORMAL     0xA6
#define SSD_CMD_DISP_INVERSE    0xA7
#define SSD_CMD_DISP_OFF        0xAE
#define SSD_CMD_DISP_ON         0xAF
#define SSD_CMD_SET_CLK_DIV     0xD5
#define SSD_CMD_SET_MULTIPLEX   0xA8
#define SSD_CMD_SET_DISP_OFFSET 0xD3
#define SSD_CMD_SET_START_LN    0x40
#define SSD_CMD_CHARGE_PUMP     0x8D
#define SSD_CMD_MEMORYMODE      0x20
#define SSD_CMD_SEGREMAP        0x01
#define SSD_CMD_COMSCAN_DEC     0xC8
#define SSD_CMD_SET_COMPINS     0xDA
#define SSD_CMD_SET_PRECHARGE   0xD9
#define SSD_CMD_SET_VCOM_DETECT 0xDB
#define SSD_CMD_SET_COL_ADDR    0x21
#define SSD_CMD_SET_PAGE_ADDR   0x22

#define SSD_SCREEN_WIDTH        128
#define SSD_SCREEN_HEIGHT       64
#define SSD_CTRL                0x00
#define SSD_DATA                0x40
#define SSD_I2C_ADDR            0x3C
#define SSD_RESET               15

// Some of this is limited for memory reasons
#define VM_MAP_SIZE             128     // 128/8 * 64/8 = 16 * 8 = 128
#define VM_MAX_SPRITES          32
#define VM_MAX_TILES            64
#define VM_NUM_REGS             32
#define VM_CMD_LEN              10
#define VM_INPUT_REG            0

namespace gamecard {
    class Ssd1306 {
        private:
            void _command(uint8_t cmd) const;
        
        public:
            void init() const;
            void test() const;
            
            void clear() const;
            void putTile(uint8_t data[8], uint8_t tileX, uint8_t tileY) const;
            void putQuadTile(
                uint8_t tiles[4][8], uint8_t topLeftX, uint8_t topLeftY
            ) const;
            void drawOffsetTile(
                uint8_t x, uint8_t y, uint8_t data[8], uint8_t bgTiles[4][8]
            ) const;
    };

    struct Sprite {
        uint8_t x, y;
        uint8_t image;
    };
    
    typedef uint8_t Image[8];
    
    enum class CompareState {
        Equal, LessThan, GreaterThan
    };

    class VirtualMachine {
        private:
            const Ssd1306 _display PROGMEM = Ssd1306();
            Image _tiles[VM_MAX_TILES];
            uint8_t _bg[VM_MAP_SIZE];
            Sprite _sprs[VM_MAX_SPRITES];
            int32_t _regs[VM_NUM_REGS];
            CompareState _cmpReg;
            
            void _copyTile(uint8_t index, uint8_t data[8]);
            void _clearSprite(uint8_t index);
            void _updateSprites();
            void _updateMap();
        
        public:
            uint64_t pc;
            
            void init();
            void execute(uint8_t command[VM_CMD_LEN]);
            void input(int32_t value);
            void testDisplay();
    };
}

#endif

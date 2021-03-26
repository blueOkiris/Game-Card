#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/spi.h>
#include <Rom.hpp>

using namespace gamecard;

M23a1024::M23a1024() {
    // Initialize spi
    spi_init(SPI_PORT, SPI_SPEED);
    
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    
    // Set to byte mode
    gpio_put(PIN_CS, 1);
    sleep_ms(50);
    gpio_put(PIN_CS, 0);
    sleep_ms(50);
    gpio_put(PIN_CS, 1);
    
    _writeReg(M23A1024_REG_BM);
}

void M23a1024::write(
        const uint32_t addr, const uint8_t *buff, const int len) const {
    uint32_t currAddr = addr;
    uint8_t cmd[5];
    Instruction inst;
    for(int i = 0; i < len; i++) {
        inst.val = (M23A1024_WRITE << 24) | (currAddr++ & 0x00FFFFFF);
        cmd[0] = inst.msb;
        cmd[1] = inst.pmsb;
        cmd[2] = inst.nlsb;
        cmd[3] = inst.lsb;
        cmd[4] = buff[i];
        /*printf("Sending: ");
        for(int j = 0; j < 5; j++) {
            printf("%d ", cmd[j]);
        }
        printf("\n");*/
        gpio_put(PIN_CS, 0);
        spi_write_blocking(SPI_PORT, cmd, 5);
        gpio_put(PIN_CS, 1);
    }
}

void M23a1024::read(
        const uint32_t addr, uint8_t *buff, const int len) const {
    uint32_t currAddr = addr;
    uint8_t cmd[4];
    Instruction inst;
    for(int i = 0; i < len; i++) {
        inst.val = (M23A1024_READ << 24) | (currAddr++ & 0x00FFFFFF);
        cmd[0] = inst.msb;
        cmd[1] = inst.pmsb;
        cmd[2] = inst.nlsb;
        cmd[3] = inst.lsb;
        gpio_put(PIN_CS, 0);
        spi_write_blocking(SPI_PORT, cmd, 4);
        spi_read_blocking(SPI_PORT, 0xFF, buff + i, 1);
        gpio_put(PIN_CS, 1);
        /*printf("Sending: ");
        for(int j = 0; j < 4; j++) {
            printf("%d ", cmd[j]);
        }
        printf(", Read: %d\n", buff[i]);*/
    }
}

void M23a1024::_writeReg(const uint8_t data) const {
    uint8_t cmd[2] = { M23A1024_WRMR, data };
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, cmd, 2);
    gpio_put(PIN_CS, 1);
}

uint64_t M23a1024::size() const {
    return 128 * 1024;
}

M23lc512::M23lc512() {
    // Initialize spi
    spi_init(SPI_PORT, SPI_SPEED);
    
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    
    // Set to byte mode
    gpio_put(PIN_CS, 1);
    sleep_ms(50);
    gpio_put(PIN_CS, 0);
    sleep_ms(50);
    gpio_put(PIN_CS, 1);
    
    uint8_t cmd[2] = { M25LC512_WRSR, 0x00 };
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, cmd, 2);
    gpio_put(PIN_CS, 1);
}

void M23lc512::write(
        const uint32_t addr, const uint8_t *buff, const int len) const {
    uint16_t currAddr = static_cast<uint16_t>(addr);
    
    Instruction inst;
    uint8_t cmd[4];
    cmd[0] = M25LC512_WRITE;
    
    for(int i = 0; i < len; i++) {
        _wren();
        
        inst.val = currAddr++;
        cmd[1] = inst.nlsb;
        cmd[2] = inst.lsb;
        
        cmd[3] = buff[i];
        
        gpio_put(PIN_CS, 0);
        spi_write_blocking(SPI_PORT, cmd, 4);
        gpio_put(PIN_CS, 1);
    }
}

void M23lc512::read(
        const uint32_t addr, uint8_t *buff, const int len) const {
    uint16_t currAddr = static_cast<uint16_t>(addr);
    
    Instruction inst;
    uint8_t cmd[3];
    cmd[0] = M25LC512_READ;
    
    for(int i = 0; i < len; i++) {
        inst.val = currAddr++;
        cmd[1] = inst.nlsb;
        cmd[2] = inst.lsb;
        
        gpio_put(PIN_CS, 0);
        spi_write_blocking(SPI_PORT, cmd, 3);
        spi_read_blocking(SPI_PORT, 0xFF, buff + i, 1);
        gpio_put(PIN_CS, 1);
    }
}

void M23lc512::_wren() const {
    uint8_t cmd = M25LC512_WREN;
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, &cmd, 1);
    gpio_put(PIN_CS, 1);
}

uint64_t M23lc512::size() const {
    return 64 * 1024;
}


#ifndef _SPI_HPP_
#define _SPI_HPP_

#define SPI_PIN_MOSI    11
#define SPI_PIN_MISO    12
#define SPI_PIN_SCK     13

namespace tester {
    class SpiBus {        
        public:
            void init() const;
            char transfer(volatile char data) const;
    };
}

#endif

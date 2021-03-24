#include <stdio.h>
#include <pico/stdlib.h>
#include <Oled.hpp>

using namespace gamecard;

int main() {
    stdio_init_all();
    printf("Welcome to gamecard!\n");
    
    const Ssd1306 oled;
    oled.test();
    
    while(1) {
        
    }
    
    return 0;
}

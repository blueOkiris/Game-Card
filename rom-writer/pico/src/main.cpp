#include <stdio.h>
#include <pico/stdlib.h>
#include <Rom.hpp>

using namespace gamecard;

int main() {
    stdio_init_all();
    const M23a1024 rom;
    
    sleep_ms(2000);
    printf("READY!");
    
    bool cont = false;
    while(!cont) {
        const auto test = getc(stdin);
        if(test == '0') {
            printf("\nReceived!\n");
            cont = true;
        }
    }
    
    return 0;
}

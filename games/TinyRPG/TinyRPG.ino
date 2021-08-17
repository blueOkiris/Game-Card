/*
 * Author: Dylan Turner
 * Description: Main entry port for tiny RPG
 */

#include <GameCardDisplay.hpp>
const gamecard::Ssd1306 g_disp;

void setup(void) {
    g_disp.init();
    g_disp.fill(false);
    delay(1000);
    g_disp.fill(true);
    delay(1000);
    while(1) {
        g_disp.test();
    }
}

void loop(void) {

}

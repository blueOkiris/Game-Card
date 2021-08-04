/*
 * Author: Dylan Turner
 * Description: Snake clone. Game ends after getting all foods (128)
 */

#include <GameCardDisplay.hpp>
#include <GameCardControls.hpp>
#include "State.hpp"
#include "Sprites.hpp"

const gamecard::Ssd1306 g_disp;
const gamecard::ButtonController g_cont;
GameState g_state;

void setup(void) {
    g_disp.init();
    g_cont.init();
}

void loop(void) {
    switch(g_state.curr) {
        case GameState::StateName::Menu: {
            const uint8_t titleMsg[4] = {
                FONT_S, FONT_N, FONT_E, FONT_K
            };
            print(titleMsg, 4, 6, 1);
        } break;

        case GameState::StateName::Game:
            break;
        
        case GameState::StateName::End:
            break;
    }
}

void print(
        uint8_t *letters, const int len,
        const uint8_t startRow, const uint8_t startCol) {
    for(int i = 0; i < len; i++) {
        g_disp.putTile(g_letters[letters[i]], startCol + i, startRow);
    }
}

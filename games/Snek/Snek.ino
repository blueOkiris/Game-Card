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
            print(titleMsg, 4, 6, 3);

            while(!g_cont.isInputPressed(gamecard::Input::High));
            randomSeed(millis());
            g_state.reset();
            g_state.curr = GameState::StateName::Game;
            g_disp.fill(true);
            drawWalls();
            drawSnake();
            drawFood();
            delay(1000);
        } break;

        case GameState::StateName::Game:
            moveSnake();
            updateSpriteIfMovedTile();
            changeDir();

            score();
            
            if(dead()) {
                delay(1000);
                g_disp.fill(true);
                g_state.curr = GameState::StateName::Dead;
            }
            if(g_state.score == 83) {
                g_state.curr = GameState::StateName::Win;
            }
            break;
        
        case GameState::StateName::Dead: {
            const uint8_t deathMsg[4] = {
                FONT_D, FONT_E, FONT_A, FONT_D
            };
            print(deathMsg, 4, 6, 2);

            const uint8_t score[2] = {
                (g_state.score / 10) % 10,
                g_state.score % 10
            };
            printNums(score, 2, 7, 4);
            
            while(!g_cont.isInputPressed(gamecard::Input::High));
            delay(1);
            while(g_cont.isInputPressed(gamecard::Input::High));
            g_state.curr = GameState::StateName::Menu;
            g_disp.fill(true);
        } break;
            
        case GameState::StateName::Win: {
            const uint8_t winMsg[3] = {
                FONT_W, FONT_I, FONT_N
            };
            print(winMsg, 3, 6, 3);
            
            while(!g_cont.isInputPressed(gamecard::Input::High));
            delay(1);
            while(g_cont.isInputPressed(gamecard::Input::High));
            g_state.curr = GameState::StateName::Menu;
            g_disp.fill(true);
        } break;
    }
}

void score(void) {
    if(g_state.tile == g_state.foodPos) {
        g_state.score++;
        g_state.plyrSpd += g_state.acc;
        
        do {
            g_state.foodPos = (random(1, 15) << 4) + random(1, 7);
        } while(g_state.foodInSnake());
        drawFood();
    }
}

void changeDir(void) {
    if(g_cont.isInputPressed(gamecard::Input::Up)) {
        g_state.plyrDir = GameState::Dir::Up;
    } else if(g_cont.isInputPressed(gamecard::Input::Down)) {
        g_state.plyrDir = GameState::Dir::Down;
    } else if(g_cont.isInputPressed(gamecard::Input::Left)) {
        g_state.plyrDir = GameState::Dir::Left;
    } else if(g_cont.isInputPressed(gamecard::Input::Right)) {
        g_state.plyrDir = GameState::Dir::Right;
    }
}

void drawWalls(void) {
    for(int i = 1; i < 15; i++) {
        g_disp.putTile(g_wallSprs[0], i, 0);
        g_disp.putTile(g_wallSprs[1], i, 7);
    }
    for(int i = 1; i < 7; i++) {
        g_disp.putTile(g_wallSprs[2], 0, i);
        g_disp.putTile(g_wallSprs[3], 15, i);
    }
}

bool dead(void) {
    if((g_state.tile >> 4) == 15 || (g_state.tile >> 4) == 0
            || (g_state.tile & 0x0F) == 7 || (g_state.tile & 0x0F) == 0) {
        return true;
    }

    for(int i = 1; i < g_state.score; i++) {
        if(g_state.tile == g_state.plyrBodyPos[i - 1]) {
            return true;
        }
    }

    return false;
}

void updateSpriteIfMovedTile(void) {
    uint8_t newTile =
        ((static_cast<uint8_t>(g_state.plyrHeadPos[0]) / 8) << 4)
        + static_cast<uint8_t>(g_state.plyrHeadPos[1]) / 8;
    if(newTile != g_state.tile) {
        // Clear the furthest body part (not head bc head will be replaced)
        g_disp.putTile(
            g_blankSpr,
            g_state.plyrBodyPos[g_state.score - 2] >> 4,
            g_state.plyrBodyPos[g_state.score - 2] & 0x0F
        );
        
        // Set body parts to follow their parent
        for(int i = g_state.score - 2; i >= 1; i--) {
            g_state.plyrBodyPos[i] = g_state.plyrBodyPos[i - 1];
        }

        // Set the first body part to follow the old head
        g_state.plyrBodyPos[0] = g_state.tile;

        // Update head
        g_disp.putTile(
            g_blankSpr, g_state.tile >> 4, g_state.tile & 0x0F
        );
        g_state.tile = newTile;

        // Draw the head and last part
        g_disp.putTile(g_snekHead, g_state.tile>> 4, g_state.tile & 0x0F);
        g_disp.putTile(
            g_snekSpr,
            g_state.plyrBodyPos[0] >> 4, g_state.plyrBodyPos[0] & 0x0F
        );
        g_disp.putTile(
            g_snekSpr,
            g_state.plyrBodyPos[g_state.score - 2] >> 4,
            g_state.plyrBodyPos[g_state.score - 2] & 0x0F
        );
    }
}

void moveSnake(void) {
    switch(g_state.plyrDir) {
        case GameState::Dir::Up:
            g_state.plyrHeadPos[1] -= g_state.plyrSpd;
            break;
        
        case GameState::Dir::Down:
            g_state.plyrHeadPos[1] += g_state.plyrSpd;
            break;
        
        case GameState::Dir::Left:
            g_state.plyrHeadPos[0] -= g_state.plyrSpd;
            break;
        
        case GameState::Dir::Right:
            g_state.plyrHeadPos[0] += g_state.plyrSpd;
            break;
    }
}

void drawSnake(void) {
    g_disp.putTile(
        g_snekHead, g_state.tile >> 4, g_state.tile & 0x0F
    );
    for(int i = 1; i < g_state.score; i++) {
        g_disp.putTile(
            g_snekSpr,
            g_state.plyrBodyPos[i - 1] >> 4,
            g_state.plyrBodyPos[i - 1] & 0x0F
        );
    }
}

void drawFood(void) {
    g_disp.putTile(g_foodSpr, g_state.foodPos >> 4, g_state.foodPos & 0x0F);
}

void printNums(
        uint8_t *nums, const int len,
        const uint8_t startCol, const uint8_t startRow) {
    char spr[8];
    for(int i = 0; i < len; i++) {
        for(int j = 0; j < 8; j++) {
            spr[j] = pgm_read_byte(g_numSprs + nums[i] * 8 + j);
        }
        g_disp.putTile(spr, startCol + i, startRow);
    }
}

void print(
        uint8_t *letters, const int len,
        const uint8_t startCol, const uint8_t startRow) {
    char spr[8];
    for(int i = 0; i < len; i++) {
        for(int j = 0; j < 8; j++) {
            spr[j] = pgm_read_byte(g_letters + letters[i] * 8 + j);
        }
        g_disp.putTile(spr, startCol + i, startRow);
    }
}

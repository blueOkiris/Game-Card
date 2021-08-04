/*
 * Author: Dylan Turner
 * Description: A pong clone for 1 and 2 players
 */

#include <GameCardDisplay.hpp>
#include <GameCardControls.hpp>
#include "State.hpp"
#include "Sprites.hpp"

const gamecard::Ssd1306 g_disp;
const gamecard::ButtonController g_cont;
GameState g_state;

/*
 * Ignore this.
 * Program was close to 100% of space,
 * so I decided to make it all the way
 * because I though it was funny
 */
const uint8_t PROGMEM wasteOfSpace[47] = { 0 };

void setup(void) {
    delay(pgm_read_byte(wasteOfSpace));
    
    g_disp.init();
    g_cont.init();
}

void loop(void) {
    switch(g_state.curr) {
        case GameState::StateName::Menu:
            g_state.reset();
            g_disp.fill(true);
            
            drawMenu();

            // Wait for player count
            while(!g_state.twoPlayer) {
                if(g_cont.isInputPressed(gamecard::Input::High)) {
                    break;
                } else if(g_cont.isInputPressed(gamecard::Input::Low)) {
                    g_state.twoPlayer = true;
                }
            }

            // Continue
            randomSeed(millis());
            g_state.curr = GameState::StateName::Game;
            resetPostScore();
            break;

        case GameState::StateName::Game:
            drawScore();
            
            drawPaddles();
            movePaddles();

            clearBall();
            moveBallAndScore();
            drawBall();
            break;

        case GameState::StateName::End:
            g_disp.fill(true);
            drawWinner();
            while(!g_cont.isInputPressed(gamecard::Input::High));
            delay(1);
            while(g_cont.isInputPressed(gamecard::Input::High));
            g_state.curr = GameState::StateName::Menu;
            break;
    }
}

bool updateMoveDirection(
        bool *pressedRef, uint8_t *heightRef, gamecard::Input inp,
        uint8_t drawX, bool isUp) {
    if(!(*pressedRef) && g_cont.isInputPressed(inp)
            && ((isUp && *heightRef > 0) || (!isUp && *heightRef < 7))) {
        g_disp.putTile(g_backSprs[0], drawX, *heightRef);

        *pressedRef = true;
        (*heightRef) += isUp ? -1 : 1;
    } else if(!g_cont.isInputPressed(inp)) {
        *pressedRef = false;
    }
}

void movePaddles() {
    // Player 1 Control
    updateMoveDirection(
        &g_state.justPressed[0], &g_state.playerHeight[0],
        gamecard::Input::Up, 0, true
    );
    updateMoveDirection(
        &g_state.justPressed[1], &g_state.playerHeight[0],
        gamecard::Input::Down, 0, false
    );

    // Player 2 Control
    if(g_state.twoPlayer) { // Manual
        updateMoveDirection(
            &g_state.justPressed[2], &g_state.playerHeight[1],
            gamecard::Input::High, 15, true
        );
        updateMoveDirection(
            &g_state.justPressed[3], &g_state.playerHeight[1],
            gamecard::Input::Low, 15, false
        );
    } else { // AI
        g_disp.putTile(g_backSprs[0], 15, g_state.playerHeight[1]);
        if(!g_state.aiPlayerDown && g_state.playerHeight[1] == 0) {
            g_state.playerHeight[1]++;
            g_state.aiPlayerDown = true;
        } else if(g_state.aiPlayerDown && g_state.playerHeight[1] == 7) {
            g_state.playerHeight[1]--;
            g_state.aiPlayerDown = false;
        } else {
            g_state.playerHeight[1] += g_state.aiPlayerDown ? 1 : -1;
        }
    }
}

void drawPaddles(void) {
    g_disp.putTile(g_paddleSpr, 0, g_state.playerHeight[0]);
    g_disp.putTile(g_paddleSpr, 15, g_state.playerHeight[1]);
}

void drawWinner(void) {
    uint8_t sprite[8];
    const int wnrRow = 3, wnrCol = 4;
    const uint8_t wnrMsg[8] = {
        FONT_P, g_state.p1Wins ? FONT_1 : FONT_2, FONT_SPACE, FONT_SPACE,
        FONT_W, FONT_I, FONT_N, FONT_S
    };
    for(int i = 0; i < 8; i++) {
        g_disp.putTile(g_fontSprs[wnrMsg[i]], wnrCol + i, wnrRow);
    }
}

void drawScore(void) {
    g_disp.putTile(g_fontSprs[FONT_0 + g_state.playerScore[0]], 2, 1);
    g_disp.putTile(g_fontSprs[FONT_0 + g_state.playerScore[1]], 13, 1);
}

void clearBall(void) {
    g_disp.putOffsetTile(
        static_cast<uint8_t>(g_state.ballPos[0]),
        static_cast<uint8_t>(g_state.ballPos[1]),
        g_backSprs[0], g_backSprs
    );
}

void moveBallAndScore(void) {
    g_state.ballPos[0] += g_state.ballVel[0];
    g_state.ballPos[1] += g_state.ballVel[1];
    
    if(g_state.ballPos[0] < 8
            && g_state.ballPos[1] < g_state.playerHeight[0] * 8 + 8
            && g_state.ballPos[1] + 8 > g_state.playerHeight[0] * 8){
        g_state.ballVel[0] = g_state.baseBallSpeed
            + (random(0, g_state.maxBallExtra * 10) / 10.0f);
        g_state.ballPos[0] = 9;
    } else if(g_state.ballPos[0] > 111
            && g_state.ballPos[1] < g_state.playerHeight[1] * 8 + 8
            && g_state.ballPos[1] + 8 > g_state.playerHeight[1] * 8) {
        g_state.ballVel[0] = -g_state.baseBallSpeed
            + (random(0, g_state.maxBallExtra * 10) / 10.0f);
        g_state.ballPos[0] = 110;
    } else if(g_state.ballPos[0] < 1) {
        g_state.playerScore[1]++;
        
        if(g_state.playerScore[1] > 7) {
            g_state.p1Wins = false;
            g_disp.fill(true);
            g_state.curr = GameState::StateName::End;
        } else {        
            resetPostScore();
        }
    }  else if(g_state.ballPos[0] > 119) {
        g_state.playerScore[0]++;
        
        if(g_state.playerScore[0] > 7) {
            g_state.p1Wins = true;
            g_disp.fill(true);
            g_state.curr = GameState::StateName::End;
        } else {
            resetPostScore();
        }
    } else if(g_state.ballPos[1] > 55) {
        g_state.ballVel[1] = -g_state.baseBallSpeed
            + (random(0, g_state.maxBallExtra * 10) / 10.0f);
        g_state.ballPos[1] = 54;
    } else if(g_state.ballPos[1] < 1) {
        g_state.ballVel[1] = g_state.baseBallSpeed
            + (random(0, g_state.maxBallExtra * 10) / 10.0f);
        g_state.ballPos[1] = 2;
    }
}

void resetPostScore(void) {
    // Reset positions
    g_state.playerHeight[0] = 3;
    g_state.playerHeight[1] = 3;
    g_state.ballPos[0] = SSD_SCREEN_WIDTH / 2 - 4;
    g_state.ballPos[1] = SSD_SCREEN_HEIGHT / 2 - 4;
    g_state.ballVel[0] =
        (g_state.baseBallSpeed
            + (random(0, g_state.maxBallExtra * 10) / 10.0f))
        * (random(0, 2) ? -1 : 1);
    g_state.ballVel[1] =
        (g_state.baseBallSpeed
            + (random(0, g_state.maxBallExtra * 10) / 10.0f))
        * (random(0, 2) ? -1 : 1);
    
    // Redraw everything
    g_disp.fill(true);
    drawScore();
    drawBall();
    delay(1000);
}

void drawBall(void) {
    g_disp.putOffsetTile(
        static_cast<uint8_t>(g_state.ballPos[0]),
        static_cast<uint8_t>(g_state.ballPos[1]),
        g_ballSpr, g_backSprs
    );
}

void drawMenu(void) {
    // Print title
    const int titleRow = 1, titleCol = 6;
    const uint8_t title[4] = {
        FONT_P, FONT_I, FONT_N, FONT_G
    };
    for(int i = 0; i < 4; i++) {
        g_disp.putTile(g_fontSprs[title[i]], titleCol + i, titleRow);
    }

    // Draw the 1 player option: 1P-H
    const int p1OpRow = 5, p1OpCol = 2;
    const uint8_t p1Op[4] = {
        FONT_1, FONT_P, FONT_DASH, FONT_H
    };
    for(int i = 0; i < 4; i++) {
        g_disp.putTile(g_fontSprs[p1Op[i]], p1OpCol + i, p1OpRow);
    }
    
    // Draw the 2 player option: 2P-L
    const int p2OpRow = 5, p2OpCol = 10;
    const uint8_t p2Op[4] = {
        FONT_2, FONT_P, FONT_DASH, FONT_L
    };
    for(int i = 0; i < 4; i++) {
        g_disp.putTile(g_fontSprs[p2Op[i]], p2OpCol + i, p2OpRow);
    }
}

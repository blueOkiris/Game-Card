/*
 * Author: Dylan Turner
 * Description: Game state for the Snek clone as well as object info
 */

#ifndef _STATE_HPP_
#define _STATE_HPP_

struct GameState {
    const float baseMoveSpd = 0.001f; // Pixels, not tiles
    const float acc = 0.0002f; // Same as above

    enum class StateName {
        Menu, Game, Dead, Win
    };

    enum class Dir {
        Up, Down, Left, Right
    };

    StateName curr;
    Dir plyrDir;
    float plyrSpd;
    float plyrHeadPos[2];
    uint8_t plyrBodyPos[128];
    uint8_t foodPos;
    uint8_t score;
    uint8_t tile;

    GameState() {
        reset();
    }

    void reset(void) {
        curr = StateName::Menu;
        score = 0;
        plyrSpd = baseMoveSpd;
        plyrDir = Dir::Right;
        plyrHeadPos[0] = SSD_SCREEN_WIDTH / 2;
        plyrHeadPos[1] = SSD_SCREEN_HEIGHT / 2;
        tile =
            ((static_cast<uint8_t>(plyrHeadPos[0]) / 8) << 4)
            + static_cast<uint8_t>(plyrHeadPos[1]) / 8;
        plyrBodyPos[0] =
            ((static_cast<uint8_t>(plyrHeadPos[0]) / 8 - 1) << 4)
            + static_cast<uint8_t>(plyrHeadPos[1]) / 8;
        plyrBodyPos[1] = 
            ((static_cast<uint8_t>(plyrHeadPos[0]) / 8 - 2) << 4)
            + static_cast<uint8_t>(plyrHeadPos[1]) / 8;
        score = 3;
        do {
            foodPos = (random(1, 15) << 4) + random(1, 7);
        } while(foodInSnake());
    }
    
    bool foodInSnake(void) {
        if(foodPos == tile) {
            return true;
        }
        for(int i = 1; i < score; i++) {
            if(foodPos == plyrBodyPos[i - 1]) {
                return true;
            }
        }
        return false;
    }
};

#endif

/*
 * Author: Dylan Turner
 * Description: Game state for the Snek clone as well as object info
 */

#ifndef _STATE_HPP_
#define _STATE_HPP_

struct GameState {
    const float baseMoveSpd = 0.75f;
    const float scoreAcc = 0.05f;

    enum class StateName {
        Menu, Game, End
    };

    enum class Dir {
        Up, Down, Left, Right
    };

    StateName curr;
    Dir plyrDir;
    float plyrSpd;
    float plyrHeadPos[2];
    //float plyrBodyPos[128][2];
    uint8_t foodPos[2];
    uint8_t score;

    GameState() {
        reset();
    }

    void reset(void) {
        curr = StateName::Menu;
        score = 0;
        plyrSpd = baseMoveSpd;
        plyrHeadPos[0] = SSD_SCREEN_WIDTH / 2;
        plyrHeadPos[1] = SSD_SCREEN_HEIGHT / 2;
        //plyrBodyPos[0][0] = plyrHeadPos[0] - 8;
        //plyrBodyPos[0][1] = plyrHeadPos[1];
        //plyrBodyPos[1][0] = plyrHeadPos[0] - 16;
        //plyrBodyPos[1][1] = plyrHeadPos[1];
        score = 3;
        foodPos[0] = random(0, 16);
        foodPos[1] = random(0, 8);
    }
};

#endif

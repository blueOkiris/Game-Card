#ifndef _STATE_HPP_
#define _STATE_HPP_

struct GameState {
    const float baseBallSpeed = 5.0f;
    const float maxBallExtra = 3.0f;

    enum class StateName {
        Menu, Game, End
    };
    
    bool twoPlayer, aiPlayerDown;
    bool p1Wins;
    float playerHeight[2]; // Player 1 and 2 each have y
    uint8_t lastAiTile;
    uint8_t playerScore[2]; // P1 & 2 each have score (up to 7)
    float ballPos[2]; // Unlike players, we want both x and y
    float ballVel[2];
    StateName curr;
    bool justPressed[4];
    
    GameState() {
        reset();
    }

    void reset(void) {
        twoPlayer = false;
        p1Wins = true;
        aiPlayerDown = false;
        playerHeight[0] = playerHeight[1] = 3;
        lastAiTile = 3;
        playerScore[0] = playerScore[1] = 0;
        ballPos[0] = SSD_SCREEN_WIDTH / 2 - 4;
        ballPos[1] = SSD_SCREEN_HEIGHT / 2 - 4;
        ballVel[0] =
            (baseBallSpeed + (random(0, maxBallExtra * 10) / 10.0f))
            * (random(0, 2) ? -1 : 1);
        ballVel[1] =
            (baseBallSpeed + (random(0, maxBallExtra * 10) / 10.0f))
            * (random(0, 2) ? -1 : 1);
        curr = StateName::Menu;
        justPressed[0] = justPressed[1] = false;
        justPressed[2] = justPressed[3] = false;
    }
};

#endif

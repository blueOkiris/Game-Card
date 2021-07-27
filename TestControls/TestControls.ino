/*
 * Author: Dylan Turner
 * Description:
 *  - Goal is to test controls
 *  - Requires some form of output to be working, i.e. the screen
 *  - Up, down, left, and right all move a cursor around
 *  - A clears the screen
 *  - B fills the screen
 */

#include <GameCardDisplay.hpp>
#include <GameCardControls.hpp>

const gamecard::Ssd1306 disp;
const gamecard::ButtonController cont;

const uint8_t sprite[8] = {
    0b00011000,
    0b00100100,
    0b01000010,
    0b10000001,
    0b10000001,
    0b01000010,
    0b00100100,
    0b00011000,
};
const uint8_t sprClr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

int x = 4, y = 4;

bool aJustPressed = false;
bool bJustPressed = false;
bool upJustPressed = false;
bool downJustPressed = false;
bool leftJustPressed = false;
bool rightJustPressed = false;

void setup() {
    cont.init();
    
    disp.init();
    disp.fill(true);
    disp.putTile(sprite, x, y);
}

void loop() {
    if(cont.isInputPressed(gamecard::Input::A) && !aJustPressed) {
        disp.fill(true);
        disp.putTile(sprite, x, y);
        aJustPressed = true;
    } else if(!cont.isInputPressed(gamecard::Input::A)) {
        aJustPressed = false;
    }
    
    if(cont.isInputPressed(gamecard::Input::B) && !bJustPressed) {
        disp.fill(false);
        bJustPressed = true;
    } else if(!cont.isInputPressed(gamecard::Input::B)) {
        bJustPressed = false;
    }
    
    if(cont.isInputPressed(gamecard::Input::Up) && !upJustPressed) {
        disp.putTile(sprClr, x, y);
        y--;
        disp.putTile(sprite, x, y);
        upJustPressed = true;
    } else if(!cont.isInputPressed(gamecard::Input::Up)) {
        upJustPressed = false;
    }
    
    if(cont.isInputPressed(gamecard::Input::Down) && !downJustPressed) {
        disp.putTile(sprClr, x, y);
        y++;
        disp.putTile(sprite, x, y);
        downJustPressed = true;
    } else if(!cont.isInputPressed(gamecard::Input::Down)) {
        downJustPressed = false;
    }
    
    if(cont.isInputPressed(gamecard::Input::Left) && !leftJustPressed) {
        disp.putTile(sprClr, x, y);
        x--;
        disp.putTile(sprite, x, y);
        leftJustPressed = true;
    } else if(!cont.isInputPressed(gamecard::Input::Left)) {
        leftJustPressed = false;
    }
    
    if(cont.isInputPressed(gamecard::Input::Right) && !rightJustPressed) {
        disp.putTile(sprClr, x, y);
        x++;
        disp.putTile(sprite, x, y);
        rightJustPressed = true;
    } else if(!cont.isInputPressed(gamecard::Input::Right)) {
        rightJustPressed = false;
    }
}

#include "player.h"

player_t player = {
    // player starts at middle of rectangular screen
    .x = WINDOW_WIDTH / 2,
    .y = WINDOW_HEIGHT / 2,
    // player size is 5x5 px^2
    .width = 5,
    .height = 5,
    .turnDirection = 0,
    .walkDirection = 0,
    .rotationAngle = PI / 2, //90 deg in radians
    .walkSpeed = 100,
    .turnSpeed = 45 * (PI / 180) //45 deg in radians
};

/* Moves player according to current delta time and 
player condition params after update()*/
void movePlayer(float deltaTime) {
    // As speeds are constant for linear and angular
    // distance is proportional to delta time
    player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
    float moveStep = player.walkDirection * player.walkSpeed * deltaTime;

    float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
    float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

    if (!mapHasWallAt(newPlayerX, newPlayerY)) {
        player.x = newPlayerX;
        player.y = newPlayerY;
    }
}

/* reders player on screen as a small rectangle */
void renderPlayer(void) {
    drawRect(    
        player.x * MINIMAP_SCALE_FACTOR,
        player.y * MINIMAP_SCALE_FACTOR,
        player.width * MINIMAP_SCALE_FACTOR,
        player.height * MINIMAP_SCALE_FACTOR,
        0xFFFFFFFF
    );
}

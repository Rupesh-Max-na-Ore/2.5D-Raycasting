#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include "defs.h"
#include "textures.h"
#include "map.h"
#include "graphics.h"

int isGameRunning = false;
int ticksLastFrame;


struct Player {
    float x;
    float y;
    float width;
    float height;
    int turnDirection; // -1 for left, +1 for right
    int walkDirection; // -1 for back, +1 for front
    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} player;

struct Ray {
    float rayAngle;
    float wallHitX;
    float wallHitY;
    float distance;
    bool wasHitVertical;
    int wallHitContent;
} rays[NUM_RAYS];




/*Initializes the start conditions of the player and color buffer*/
void setup(void) {
    // player starts at middle of rectangular screen
    player.x = WINDOW_WIDTH / 2;
    player.y = WINDOW_HEIGHT / 2;
    // player size is 5x5 px^2
    player.width = 5;
    player.height = 5;
    player.turnDirection = 0;
    player.walkDirection = 0;
    player.rotationAngle = PI / 2; //90 deg in radians
    player.walkSpeed = 100;
    player.turnSpeed = 45 * (PI / 180); //45 deg in radians

    // Ask uPNG library to decode all PNG files and loads the wallTextures array
    loadWallTextures();
}


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
    /*
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect playerRect = {
        player.x * MINIMAP_SCALE_FACTOR,
        player.y * MINIMAP_SCALE_FACTOR,
        player.width * MINIMAP_SCALE_FACTOR,
        player.height * MINIMAP_SCALE_FACTOR
    };
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_RenderDrawLine(
        renderer,
        MINIMAP_SCALE_FACTOR * player.x,
        MINIMAP_SCALE_FACTOR * player.y,
        MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 40,
        MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle) * 40
    );
    */
}

// Normalize incresing angle to b/w [0,2pi] radians
float normalizeAngle(float angle) {
    angle = remainder(angle, TWO_PI);
    if (angle < 0) {
        angle = TWO_PI + angle;
    }
    return angle;
}

// To Find Euclidean Distance
float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

/* Cast a ray upto point of nearest collision with obstacle or boundary*/
void castRay(float rayAngle, int stripId) {
    rayAngle = normalizeAngle(rayAngle);
    
    int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
    int isRayFacingUp = !isRayFacingDown;

    int isRayFacingRight = rayAngle < 0.5 * PI || rayAngle > 1.5 * PI;
    int isRayFacingLeft = !isRayFacingRight;
    
    // Need to calculate nerest horizontal and vertical collisions
    // and take minimum out of them for finding the ray's end
    
    float xintercept, yintercept;
    float xstep, ystep;

    ///////////////////////////////////////////
    // HORIZONTAL RAY-GRID INTERSECTION CODE
    ///////////////////////////////////////////
    bool foundHorzWallHit = false;
    float horzWallHitX = 0;
    float horzWallHitY = 0;
    int horzWallContent = 0;

    // Find the y-coordinate of the closest horizontal grid intersection
    yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
    yintercept += isRayFacingDown ? TILE_SIZE : 0;

    // Find the x-coordinate of the closest horizontal grid intersection
    xintercept = player.x + (yintercept - player.y) / tan(rayAngle);

    // Calculate the increment xstep and ystep
    ystep = TILE_SIZE;
    ystep *= isRayFacingUp ? -1 : 1;

    xstep = TILE_SIZE / tan(rayAngle);
    xstep *= (isRayFacingLeft && xstep > 0) ? -1 : 1;
    xstep *= (isRayFacingRight && xstep < 0) ? -1 : 1;

    float nextHorzTouchX = xintercept;
    float nextHorzTouchY = yintercept;

    // Increment xstep and ystep until we find a wall //Make window and Map Independent
    while (isInsideMap(nextHorzTouchX, nextHorzTouchY)) {
        float xToCheck = nextHorzTouchX;
        float yToCheck = nextHorzTouchY + (isRayFacingUp ? -1 : 0);
        
        if (mapHasWallAt(xToCheck, yToCheck)) {
            // found a wall hit
            horzWallHitX = nextHorzTouchX;
            horzWallHitY = nextHorzTouchY;
            horzWallContent = getMapAt((int)floor(yToCheck / TILE_SIZE), (int)floor(xToCheck / TILE_SIZE));
            foundHorzWallHit = true;
            break;
        } else {
            nextHorzTouchX += xstep;
            nextHorzTouchY += ystep;
        }
    }
    
    ///////////////////////////////////////////
    // VERTICAL RAY-GRID INTERSECTION CODE
    ///////////////////////////////////////////
    bool foundVertWallHit = false;
    float vertWallHitX = 0;
    float vertWallHitY = 0;
    int vertWallContent = 0;

    // Find the x-coordinate of the closest vertical grid intersection
    xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
    xintercept += isRayFacingRight ? TILE_SIZE : 0;

    // Find the y-coordinate of the closest vertical grid intersection
    yintercept = player.y + (xintercept - player.x) * tan(rayAngle);

    // Calculate the increment xstep and ystep
    xstep = TILE_SIZE;
    xstep *= isRayFacingLeft ? -1 : 1;

    ystep = TILE_SIZE * tan(rayAngle);
    ystep *= (isRayFacingUp && ystep > 0) ? -1 : 1;
    ystep *= (isRayFacingDown && ystep < 0) ? -1 : 1;

    float nextVertTouchX = xintercept;
    float nextVertTouchY = yintercept;

    // Increment xstep and ystep until we find a wall
    while (isInsideMap(nextVertTouchX, nextVertTouchY)){
        float xToCheck = nextVertTouchX + (isRayFacingLeft ? -1 : 0);
        float yToCheck = nextVertTouchY;
        
        if (mapHasWallAt(xToCheck, yToCheck)) {
            // found a wall hit
            vertWallHitX = nextVertTouchX;
            vertWallHitY = nextVertTouchY;
            vertWallContent = getMapAt((int)floor(yToCheck / TILE_SIZE), (int)floor(xToCheck / TILE_SIZE));
            foundVertWallHit = true;
            break;
        } else {
            nextVertTouchX += xstep;
            nextVertTouchY += ystep;
        }
    }

    // Calculate both horizontal and vertical hit distances and choose the smallest one
    float horzHitDistance = foundHorzWallHit
        ? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY)
        : FLT_MAX;
    float vertHitDistance = foundVertWallHit
        ? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
        : FLT_MAX;

    if (vertHitDistance < horzHitDistance) {
        rays[stripId].distance = vertHitDistance;
        rays[stripId].wallHitX = vertWallHitX;
        rays[stripId].wallHitY = vertWallHitY;
        rays[stripId].wallHitContent = vertWallContent;
        rays[stripId].wasHitVertical = true;
        rays[stripId].rayAngle = rayAngle;
    } else {
        rays[stripId].distance = horzHitDistance;
        rays[stripId].wallHitX = horzWallHitX;
        rays[stripId].wallHitY = horzWallHitY;
        rays[stripId].wallHitContent = horzWallContent;
        rays[stripId].wasHitVertical = false;
        rays[stripId].rayAngle = rayAngle;
    }
}

/* Cast each ray*/
// void castAllRays() {
//     //Start from the lowest angle ray, span upto fov angle
//     //i.e., angle of rays lie w/in rangle of [-fov/2, +fov/2] wrt player rotation angle
//     float rayAngle = player.rotationAngle - (FOV_ANGLE / 2);

//     for (int stripId = 0; stripId < NUM_RAYS; stripId++) {
//         castRay(rayAngle, stripId);
//         rayAngle += FOV_ANGLE / NUM_RAYS; //angle of next ray
//     }
// }
/* Corrected to fix curvy wall distortion, 
fixing ray distance intervals from wall,
then computing angle for that interval using trigonometry */
void castAllRays(void) {
    for (int col = 0; col < NUM_RAYS; col++) {
        float rayAngle = player.rotationAngle + atan((col - NUM_RAYS/2) / DIST_PROJ_PLANE);
        castRay(rayAngle, col);
    }
}




/* Reders all Rays coming out of Player*/
void renderRays(void) {
    /*
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < NUM_RAYS; i++) {
        SDL_RenderDrawLine(
            renderer,
            MINIMAP_SCALE_FACTOR * player.x,
            MINIMAP_SCALE_FACTOR * player.y,
            MINIMAP_SCALE_FACTOR * rays[i].wallHitX,
            MINIMAP_SCALE_FACTOR * rays[i].wallHitY
        );
    }
    */
}

/*Listens and Checks user input */
void processInput(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT: {
            isGameRunning = false;
            break;
        }
        case SDL_KEYDOWN: {
            if (event.key.keysym.sym == SDLK_ESCAPE)
                isGameRunning = false;
            if (event.key.keysym.sym == SDLK_UP)
                player.walkDirection = +1;
            if (event.key.keysym.sym == SDLK_DOWN)
                player.walkDirection = -1;
            if (event.key.keysym.sym == SDLK_RIGHT)
                player.turnDirection = +1;
            if (event.key.keysym.sym == SDLK_LEFT)
                player.turnDirection = -1;
            break;
        }
        case SDL_KEYUP: {
            if (event.key.keysym.sym == SDLK_UP)
                player.walkDirection = 0;
            if (event.key.keysym.sym == SDLK_DOWN)
                player.walkDirection = 0;
            if (event.key.keysym.sym == SDLK_RIGHT)
                player.turnDirection = 0;
            if (event.key.keysym.sym == SDLK_LEFT)
                player.turnDirection = 0;
            break;
        }
    }
}

/*updates the image on game screen frame by frame, as per p;ayer input*/
void update(void) {
    // Compute how long we have until the reach the target frame time in milliseconds
    int timeToWait = FRAME_TIME_LENGTH - (SDL_GetTicks() - ticksLastFrame);

    // Only delay execution if we are running too fast
    if (timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH) {
        SDL_Delay(timeToWait);
    }

    /*
    //above is equivalent to below while loop, conceptually, but doesn't block CPU for entire time
    // waste some time until we reach the target frame time length
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksLastFrame + FRAME_TIME_LENGTH));
    */

    float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

    // Store the milliseconds of the current frame to be used in the future
    ticksLastFrame = SDL_GetTicks();

    // TODO: remember to update game objects as a function of deltaTime
    movePlayer(deltaTime);
    castAllRays();
}

/* Generate 3d walls projection from rays coming from player */
void renderWallProjection(void) {
    for (int x = 0; x < NUM_RAYS; x++) {
        // counter fishbowl effect using perpendicular wall distance
        float perpDistance = rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle);
        float projectedWallHeight = (TILE_SIZE / perpDistance) * DIST_PROJ_PLANE;

        int wallStripHeight = (int)projectedWallHeight;

        int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
        wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

        int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
        wallBottomPixel = wallBottomPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomPixel;

        // set the color of the ceiling
        for (int y = 0; y < wallTopPixel; y++)
            drawPixel(x, y, 0xFF444444);

        
        // calculate texture offset X
        int textureOffsetX;
        if (rays[x].wasHitVertical)
            textureOffsetX = (int)rays[x].wallHitY % TILE_SIZE;
        else
            textureOffsetX = (int)rays[x].wallHitX % TILE_SIZE;

        // get the correct texture id number from the map content
        int texNum = rays[x].wallHitContent - 1;

        int texture_width = wallTextures[texNum].width;
        int texture_height = wallTextures[texNum].height;

        // render the wall from wallTopPixel to wallBottomPixel
        for (int y = wallTopPixel; y < wallBottomPixel; y++) {
            int distanceFromTop = y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2);
            int textureOffsetY = distanceFromTop * ((float)texture_height / wallStripHeight);

            // set the color of the wall based on the color from the texture
            uint32_t texelColor = wallTextures[texNum].texture_buffer[(texture_width * textureOffsetY) + textureOffsetX];
            drawPixel(x, y, texelColor);
        }

        // set the color of the floor
        for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++)
            drawPixel(x, y, 0xFF888888);
    }
}



/* Renders the screen*/
void render(void) {

    // set all color buffer values to black
    clearColorBuffer(0xFF000000);


    // render 3D wall Projection
    renderWallProjection();

    renderMap();
    // renderRays();
    // renderPlayer();
    renderColorBuffer();
}

void releaseResources(void) {
    freeWallTextures();
    destroyWindow();
}

// Driver function
int main() {
    isGameRunning = initializeWindow();

    setup();

    while (isGameRunning) {
        processInput();
        update();
        render();
    }

    destroyWindow();

    return 0;
}

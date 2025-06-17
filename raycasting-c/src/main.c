#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include "defs.h"
#include "textures.h"
#include "map.h"
#include "graphics.h"
#include "player.h"
#include "ray.h"
#include "wall.h"

int isGameRunning = false;
int ticksLastFrame;


/*Initializes the start conditions of the player and color buffer*/
void setup(void) {
    // Ask uPNG library to decode all PNG files and loads the wallTextures array
    loadWallTextures();
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




/* Renders the screen*/
void render(void) {

    // set all color buffer values to black
    clearColorBuffer(0xFF000000);


    // render 3D wall Projection
    renderWallProjection();

    renderMap();
    renderRays();
    renderPlayer();
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

    releaseResources();

    return EXIT_SUCCESS;
}

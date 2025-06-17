#include "graphics.h"

// Declare as static to make sure only this file's code can modify below variables
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static uint32_t* colorBuffer = NULL;
static SDL_Texture* colorBufferTexture;

/*Initializes the window screen where game is going run*/
bool initializeWindow() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    /*Get current resolution of display, 
    scale and stretch to resolution we want*/
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int fullScreenWidth = display_mode.w;
    int fullScreenHeight = display_mode.h;
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        fullScreenWidth,
        fullScreenHeight,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // allocate the total amount of bytes in memory to hold our colorbuffer
    colorBuffer = (uint32_t*)malloc(sizeof(uint32_t) * WINDOW_WIDTH * WINDOW_HEIGHT);

    // create an SDL_Texture to display the colorbuffer
    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32, //A is alpha for transparency
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    return true;
}

/*After Quiting the game window, free memory*/
void destroyWindow() {
    free(colorBuffer);
    SDL_DestroyTexture(colorBufferTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Reset color buffer to color param for each cell
void clearColorBuffer(uint32_t color) {
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
        colorBuffer[i] = color;
}

// Render color buffer as per the color pixel values in array pointed to by colorBuffer
void renderColorBuffer() {
    SDL_UpdateTexture(
        colorBufferTexture,
        NULL,
        colorBuffer,
        (int)(WINDOW_WIDTH * sizeof(uint32_t))
    );
    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}


void drawPixel(int x, int y, uint32_t color) {
    colorBuffer[(WINDOW_WIDTH * y) + x] = color;
}

// Draws a rectangle from (x,y) coord of screen
// of size width x height, of colour = color
void drawRect(int x, int y, int width, int height, uint32_t color) {
    for (int i = x; i <= (x + width); i++) {
        for (int j = y; j <= (y + height); j++) {
            drawPixel(i, j, color);
        }
    }
}
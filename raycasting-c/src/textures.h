#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdint.h>
#include "constants.h"
#include "upng.h"

typedef struct {
    upng_t* upngTexture;
    int width;
    int height;
    uint32_t* texture_buffer;
} texture_t;

// Declaration only
extern texture_t wallTextures[NUM_TEXTURES];
/*Using extern tells the compiler "this variable exists, 
but it’s defined elsewhere," so no multiple definitions occur 
during linking. You define it once in a .c file to allocate memory for it.*/

void loadWallTextures();
void freeWallTextures();

#endif

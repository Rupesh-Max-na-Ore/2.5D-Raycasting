#include "wall.h"

// Change the color intensity based on a factor value between 0 and 1
void changeColorIntensity(color_t* color, float factor) {
    // use bit mask to extract r,g,b parts of colour and scale down, then combin back
    color_t a = (*color & 0xFF000000);
    color_t r = (*color & 0x00FF0000) * factor;
    color_t g = (*color & 0x0000FF00) * factor;
    color_t b = (*color & 0x000000FF) * factor;

    *color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
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
            color_t texelColor = wallTextures[texNum].texture_buffer[(texture_width * textureOffsetY) + textureOffsetX];
            
            // Shadow effect
            // Make the pixel color darker if the ray hit was vertical
            if (rays[x].wasHitVertical) {
                changeColorIntensity(&texelColor, 0.7);
            }
            
            drawPixel(x, y, texelColor);
        }


        // set the color of the floor
        for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++)
            drawPixel(x, y, 0xFF888888);
    }
}


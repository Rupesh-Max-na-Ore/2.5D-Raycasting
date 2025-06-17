#include <stdint.h>

#define PI 3.14159265
#define TWO_PI 6.28318530

#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20
#define NUM_TEXTURES 8

#define MINIMAP_SCALE_FACTOR 0.3 

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

#define FOV_ANGLE (60 * (PI / 180))

#define NUM_RAYS WINDOW_WIDTH

#define DIST_PROJ_PLANE ((WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2))

typedef uint32_t color_t;

#define FPS 30
#define FRAME_TIME_LENGTH (1000 / FPS)

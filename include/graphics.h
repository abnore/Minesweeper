#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "constants.h"
#include <SDL.h>

extern SDL_Window *window;
extern SDL_Renderer *renderer;

typedef struct Cell {
    int index;
    int isBomb;
    int isRevealed;
    int closeBombs;
} Cell;

extern int bombCounter;
extern int smileyExpression;
extern int lastFrameTime;

typedef struct Sprite
{
    SDL_Texture *texture;
} Sprite;

typedef struct SpriteCoordinates {
    int x, y, w, h;
} SpriteCoordinates;

extern Cell grid[GRIDSIZE][GRIDSIZE];

extern Sprite sprite;
extern SpriteCoordinates sprites[];

int initialize_window(void);

int countNeighboringBombs(int x, int y);

SDL_Texture *LoadTexture(const char *filepath, SDL_Renderer *renderer);

void drawTile(int index, int draw_x, int draw_y);

void render(void);

void revealTiles(Cell grid[GRIDSIZE][GRIDSIZE], int x, int y);

#endif
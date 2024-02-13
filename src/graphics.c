#include <graphics.h>
#include "constants.h"

int WINDOW_HEIGHT = TILESIZE * GRIDSIZE;
int WINDOW_WIDTH = TILESIZE * GRIDSIZE;

int initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != FALSE)
    {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return FALSE;
    }

    window = SDL_CreateWindow(
        "SDL Application",      // Title of the window
        SDL_WINDOWPOS_CENTERED, // Initial x position
        SDL_WINDOWPOS_CENTERED, // Initial y position
        WINDOW_WIDTH,           // Width, in pixels
        WINDOW_HEIGHT,          // Height, in pixels
        SDL_WINDOW_SHOWN);

    if (!window){
        fprintf(stderr, "Error creating SDL window: %s\n", SDL_GetError());
        return FALSE;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer){
        fprintf(stderr, "Error creating SDL renderer: %s\n", SDL_GetError());
        return FALSE;
    }

    return TRUE;
}

SDL_Texture *LoadTexture(const char *filepath, SDL_Renderer *renderer)
{
    SDL_Surface *surface = SDL_LoadBMP(filepath);
    if (!surface)
    {
        fprintf(stderr, "Error loading image: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture)
    {
        fprintf(stderr, "Error creating texture: %s\n", SDL_GetError());
    }
    return texture;
}

// Initializing sprites as an array of arrays of coordinates
SpriteCoordinates sprites[] = {

    {0, 49, 16, 16},   // 0 - normal tile
    {17, 49, 16, 16},  // 1 - pressed tile
    {34, 49, 16, 16},  // 2 - flag tile
    {51, 49, 16, 16},  // 3 - question mark
    {68, 49, 16, 16},  // 4 - pressed q mark
    {85, 49, 16, 16},  // 5 - bomb
    {102, 49, 16, 16}, // 6 - red bomb
    {119, 49, 16, 16}, // 7 - bomb cross

    {0, 66, 16, 16},   // 8 - 1
    {17, 66, 16, 16},   // 9 - 2
    {34, 66, 16, 16},  // 10 - 3
    {51, 66, 16, 16},  // 11 - 4
    {68, 66, 16, 16},  // 12 - 5
    {85, 66, 16, 16},  // 13 - 6
    {102, 66, 16, 16},  // 14 - 7
    {119, 66, 16, 16},  // 15 - 8

    {126, 0, 13, 23},  // 16 - 0 Large red numbers
    {0, 0, 13, 23},    // 17 - 1
    {14, 0, 13, 23},   // 18 - 2
    {28, 0, 13, 23},   // 19 - 3
    {42, 0, 13, 23},   // 20 - 4
    {56, 0, 13, 23},   // 21 - 5
    {70, 0, 13, 23},   // 22 - 6
    {84, 0, 13, 23},   // 23 - 7
    {98, 0, 13, 23},   // 24 - 8
    {112, 0, 13, 23},  // 25 - 9
    {140, 0, 13, 23},  // 26 - -
    {154, 0, 13, 23},  // 27 - blank

    {0, 24, 24, 24},   // 28 - Smiley Face
    {25, 24, 24, 24},  // 29 - Smiley Face pressed
    {50, 24, 24, 24},  // 30 - Shocked face
    {75, 24, 24, 24},  // 31 - Sunglassed
    {100, 24, 24, 24}, // 32 - Dead
};

void drawTile(int index, int draw_x, int draw_y)
{
    SDL_Rect srcRect;
    srcRect.x = sprites[index].x;          // X position on the texture
    srcRect.y = sprites[index].y;          // Y position on the texture
    srcRect.w = sprites[index].w;          // Width of the texture area
    srcRect.h = sprites[index].h;          // Height of the texture area

    SDL_Rect destRect;
    destRect.x = draw_x;  
    destRect.y = draw_y; 
    destRect.w = sprites[index].w * MULTIPLIER; 
    destRect.h = sprites[index].h * MULTIPLIER; 
    // Draw the texture part on the screen
    SDL_RenderCopy(renderer, sprite.texture, &srcRect, &destRect);
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0xd0, 0xd0, 0xd0, 255); // Set to white or any other color
    SDL_RenderClear(renderer);

    // Draw Smileyface
    drawTile(28 + smileyExpression, WINDOW_WIDTH / 2 - sprites[28].w, 0);

    // Draw Grid of Tiles
    for (int y = 2; y < GRIDSIZE; y++){
        for (int x = 0; x < GRIDSIZE; x++){
            drawTile(grid[y][x].index, x * TILESIZE, y * TILESIZE);
        }
    }

    // Draw counter left
    if(bombCounter < 100 && bombCounter >= 0){
        drawTile(27, 0, 0);
    } else if (bombCounter < 0){
        drawTile(26, 0, 0);
    } 
    
    if (bombCounter >= 10 ){
        drawTile((bombCounter / 10)+16, sprites[25].w*MULTIPLIER, 0);
    } else {
        drawTile((-bombCounter /10)+16, sprites[25].w * MULTIPLIER, 0);
    }

    if(bombCounter > 0){
        drawTile(16 + (bombCounter % 10), sprites[25].w*MULTIPLIER*2, 0);
    } else {
        drawTile(16 + (-bombCounter % 10), sprites[25].w * MULTIPLIER*2, 0);
    }

    // Draw counter right
    drawTile(27, WINDOW_WIDTH - 3 * sprites[27].w * MULTIPLIER, 0);
    drawTile(27, WINDOW_WIDTH - 2 * sprites[27].w * MULTIPLIER, 0);
    drawTile(27, WINDOW_WIDTH - sprites[27].w * MULTIPLIER, 0);

    SDL_RenderPresent(renderer);
}

int countNeighboringBombs(int x, int y){
    int bombCount = 0;
    for (int i = -1; i <= 1; i++){
        for (int j = -1; j <= 1; j++){
            int nx = x + i;
            int ny = y + j;

            // Check if neighbor is within grid bounds
            if (nx >= 0 && nx < GRIDSIZE && ny >= 0 && ny < GRIDSIZE){
                // Check if neighbor is a bomb
                if (grid[ny][nx].isBomb){
                    bombCount++;
                }
            }
        }
    }
    return bombCount;
}

void revealTiles(Cell grid[GRIDSIZE][GRIDSIZE], int x, int y){
    // Check if the tile coordinates are within the grid bounds
    if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
        return;

    // Check if the tile is already revealed or is a bomb
    if (grid[y][x].isRevealed || grid[y][x].isBomb)
        return;

    // Reveal this tile
    grid[y][x].isRevealed = TRUE;

    // If the tile is not blank (has neighboring bombs), stop recursion
    if (grid[y][x].closeBombs > 0)
        return;

    // Recursively reveal adjacent tiles
    for (int i = -1; i <= 1; i++){
        for (int j = -1; j <= 1; j++){
            if (i != 0 || j != 0){ // Avoid the current tile itself
                revealTiles(grid, x + i, y + j);
            }
        }
    }
}

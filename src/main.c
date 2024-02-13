#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "graphics.h"
#include "constants.h"

int window_open = FALSE;
int restart = FALSE;
int mouseX;
int mouseY;
int gridX;
int gridY;

int smileyExpression;
int totalBombs;
int bombCounter;
int totalRevealed;
int totalCells;
int lastFrameTime;

int gameOver;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// Initializing the sprite
Sprite sprite;
Cell grid[GRIDSIZE][GRIDSIZE];

void initializeGrid(Cell grid[GRIDSIZE][GRIDSIZE]) {
    // Seed the random number generator
    srand(time(NULL));
    smileyExpression = SMILEY;
    totalBombs = 0;
    totalCells = 0;
    bombCounter = 0;
    gameOver = FALSE;

    for (int y = 2; y < GRIDSIZE; y++){
        for (int x = 0; x < GRIDSIZE; x++) {
            grid[y][x].isBomb = (rand() % 5 == 0); // expression is true 1 out of 5 times
                if (grid[y][x].isBomb) totalBombs++, bombCounter++;
            grid[y][x].isRevealed = FALSE;
            grid[y][x].index = 0;
            totalCells++;

        }
    }
    for (int y = 2; y < GRIDSIZE; y++){
        for (int x = 0; x < GRIDSIZE; x++){
            if(!grid[y][x].isBomb)
            grid[y][x].closeBombs = countNeighboringBombs(x, y);
        }
    }
    printf("Total bombs: %i\n", totalBombs);
}

void process_input(){
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

        switch (event.type){

            case SDL_QUIT:
                window_open = FALSE;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    window_open = FALSE;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                
                mouseX = event.button.x;
                mouseY = event.button.y;

                gridX = mouseX / TILESIZE;
                gridY = mouseY / TILESIZE;

                if(event.button.button == SDL_BUTTON_LEFT){
                    if (gridX >= 0 && gridX < GRIDSIZE && gridY >= 2 && gridY < GRIDSIZE && grid[gridY][gridX].index != 2){
                        smileyExpression = SHOCKED;

                    } 
                    if (mouseX >= 136 && mouseX <= 183 && mouseY >= 0 && mouseY <= 50){
                            smileyExpression = PRESSED;       
                    }

                } else if (event.button.button == SDL_BUTTON_RIGHT){
                    if (gridX >= 0 && gridX < GRIDSIZE && gridY >= 2 && gridY < GRIDSIZE){

                       if( grid[gridY][gridX].index == 0) {
                           grid[gridY][gridX].index = 2;
                           if (!grid[gridY][gridX].isRevealed)
                               bombCounter--;
                       }
                       else if (grid[gridY][gridX].index == 2){
                           grid[gridY][gridX].index = 3;
                       }
                       else if (grid[gridY][gridX].index == 3){
                           grid[gridY][gridX].index = 0;
                           bombCounter++;
                       }
                    }

                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.state == SDL_RELEASED && event.button.button != SDL_BUTTON_RIGHT){
                    
                    smileyExpression = SMILEY;

                    if (gridX >= 0 && gridX < GRIDSIZE && gridY >= 2 && gridY < GRIDSIZE ){
                        
                        revealTiles(grid, gridX, gridY);
                        
                        if (grid[gridY][gridX].isBomb){
                            gameOver = TRUE;
                            grid[gridY][gridX].index = 6;
                            grid[gridY][gridX].isRevealed = TRUE;
                            smileyExpression = DEAD;
                    }
                }
                    if (mouseX >= 136 && mouseX <= 183 && mouseY >= 0 && mouseY <= 50){
                            restart = TRUE;
                    }
                break;
            }
        }
    }
}



void setup() {
    // Load your sprite image
    sprite.texture = LoadTexture("lib/Sprite.bmp", renderer); 
    // Make the grid of cells
    initializeGrid(grid);
}

void update(){
    totalRevealed = 0;

    lastFrameTime = SDL_GetTicks()/1000;

    for (int y = 2; y < GRIDSIZE; y++){
        for (int x = 0; x < GRIDSIZE; x++){
            
            if(gameOver && grid[gridY][gridX].index != 6)
                grid[gridY][gridX].isRevealed = TRUE;

            if(grid[y][x].isRevealed == TRUE){
                totalRevealed++;

                if (grid[y][x].isBomb == TRUE && gameOver){
                    grid[y][x].index = 5;

                } else {
                    if (grid[y][x].closeBombs){
                     grid[y][x].index = grid[y][x].closeBombs + 7;
                    } else {
                     grid[y][x].index = 1;
                    }
                }
             }
            //// Debugging code - to mark bombs without revealing them
            //  else if (!grid[y][x].isRevealed && grid[y][x].isBomb == TRUE && grid[y][x].index != 2){
            //      grid[y][x].index = 3;
            //  }
        }
    }
    if (totalRevealed == (totalCells - totalBombs)) {
        smileyExpression = SUNGLASSES;
    }
}
void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    window_open = initialize_window();

    if (window_open) {
        setup();
    }

    while (window_open) {

        if(restart) {
            restart = FALSE;
            setup();
        }
        process_input();
        update();
        render();

    }

    destroy_window();

    return FALSE;
}
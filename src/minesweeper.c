#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define ROW 16
#define COL 16
#define GRIDSIZE 16
#define CELL_SIZE 24
#define TILE_SIZE 16
#define WINDOW_HEIGHT 492
#define WINDOW_WIDTH 426
#define BOMB_CHANCE 10
#define CANVAS_X 21
#define CANVAS_Y 87

#define abs(a) ( (a < 0) ? -a : a )

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *icon;
SDL_Texture *background_texture, *numbers_texture, *tiles_texture, *faces_texture;

typedef enum {
	FALSE,
	TRUE,
} Bool;

typedef enum {
	GAME_OVER,
	PLAYING,
	RESTARTING,
	WON,
} GameState;

typedef struct {
	int x, y;
} Sprite;

Sprite sprites[] = {
	{17, 0},	// 0 - pressed tile
	{0, 17},	// 1 - 1
	{17, 17},	// 2 - 2
	{34, 17},	// 3 - 3
	{51, 17},	// 4 - 4
	{68, 17},	// 5 - 5
	{85, 17},	// 6 - 6
	{102, 17},	// 7 - 7
	{119, 17},	// 8 - 8
	{0, 0},		// 9 - normal tile
	{34, 0},	// 10 - flag tile
	{51, 0},	// 11 - question mark
	{68, 0},	// 12 - pressed q mark
	{85, 0},	// 13 - bomb
	{102, 0},	// 14 - red bomb
	{119, 0},	// 15 - bomb cross
	{126, 0},	// 16 - 0 Large red numbers
	{0, 0},		// 17 - 1
	{14, 0},	// 18 - 2
	{28, 0},	// 19 - 3
	{42, 0},	// 20 - 4
	{56, 0},	// 21 - 5
	{70, 0},	// 22 - 6
	{84, 0},	// 23 - 7
	{98, 0},	// 24 - 8
	{112, 0},	// 25 - 9
	{140, 0},	// 26 - -
	{154, 0},	// 27 - blank
	{0, 0},		// 28 - Smiley Face
	{25, 0},	// 29 - Smiley Face presserenderer, faces_textured
	{50, 0},	// 30 - Shocked face
	{75, 0},	// 31 - Sunglassed
	{100, 0},	// 32 - Dead
};

typedef enum {
	TILE_PRESSED,
	ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT,
	TILE_NORMAL = 9,
	TILE_FLAG,
	TILE_QUESTION,
	TILE_QUESTION_PRESSED,
	BOMB_NORMAL,
	BOMB_RED,
	BOMB_CROSS,
	FACE_NORMAL = 28,
	FACE_PRESSED,
	FACE_SHOCK,
	FACE_GLASSES,
	FACE_DEAD,
} Tiles;

typedef struct {
	SDL_Rect srcRect;
	SDL_Rect destRect;
	Tiles tile;
} Rect;

typedef struct {
	Bool isBomb;
	Bool isRevealed;
	Bool isFlagged;
	Bool isPressed;
	Rect draw;
	int closeBombs;
} Cell;

int InitWindowAndIMG(void)
{
	uint32_t WINDOW_FLAGS		=  SDL_INIT_VIDEO	| SDL_INIT_EVENTS; // apparently optional..
	uint32_t IMG_FLAGS			=  IMG_INIT_JPG		| IMG_INIT_PNG;

	if (SDL_Init(WINDOW_FLAGS) != 0){
		fprintf(stderr, "ERROR: SDL_Init with the error %s\n", SDL_GetError());
		return 0;
	}

	if (IMG_Init(IMG_FLAGS) == 0) {
		fprintf(stderr, "ERROR: IMG_Init with the error %s\n", SDL_GetError());
		return 0;
	}
	return 1;
}

void createWindow(void)
{
	window = SDL_CreateWindow(
				"Minesweeper",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				WINDOW_WIDTH,
				WINDOW_HEIGHT,
				SDL_WINDOW_SHOWN );
	if (!window) {
		fprintf(stderr, "ERROR: SDL_CreateWindow with the error %s\n", SDL_GetError());
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		fprintf(stderr, "ERROR: SDL_CreateRenderer with the error %s\n", SDL_GetError());
	}

	icon = IMG_Load("../img/icon.png");
	if (!icon) {
		fprintf(stderr, "ERROR: IMG_Load with the error %s\n", SDL_GetError());
	}

	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);

}

SDL_Texture *createTexture(SDL_Renderer *renderer, const char *file_path)
{
	SDL_Surface *surface = IMG_Load(file_path);
	if (!surface) {
		fprintf(stderr, "ERROR: SDL_Surface with the error %s\n", SDL_GetError());
		return 0;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		fprintf(stderr, "ERROR: SDL_Texture with the error %s\n", SDL_GetError());
		return 0;
	}
	SDL_FreeSurface(surface);

	return texture;
}

void loadTextures(void)
{
	background_texture = createTexture(renderer, "/../img/Sprites/background.png");
	tiles_texture = createTexture(renderer, "/../img/Sprites/tiles.png");
	numbers_texture = createTexture(renderer, "/../img/Sprites/numbers.png");
	faces_texture =	createTexture(renderer, "/../img/Sprites/faces.png");
}

void initCell(Cell *cell, int row, int col)
{
	cell->closeBombs = 0;
	cell->draw.tile = TILE_NORMAL;
	cell->isFlagged = FALSE;
	cell->isBomb = (arc4random() % BOMB_CHANCE == 0);
	cell->isRevealed = FALSE;
	cell->isPressed = FALSE;
	cell->draw.srcRect.w = TILE_SIZE;
	cell->draw.srcRect.h = TILE_SIZE;
	cell->draw.destRect.x = row * CELL_SIZE + CANVAS_X;
	cell->draw.destRect.y = col * CELL_SIZE + CANVAS_Y;
	cell->draw.destRect.w = CELL_SIZE;
	cell->draw.destRect.h = CELL_SIZE;
}


int countNeighboringBombs(Cell grid[COL][ROW], int x, int y)
{
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

void initGrid(Cell (*grid)[COL][ROW], int *num_bombs)
{
	*num_bombs = 0;

	for(int col = 0; col < COL; col++) {
		for(int row = 0; row < ROW; row++) {
			initCell(&(*grid)[col][row], row, col);
			if((*grid)[col][row].isBomb == TRUE) (*num_bombs)++;
		}
	}
	for(int col = 0; col < COL; col++) {
		for(int row = 0; row < ROW; row++) {
			if(!(*grid)[col][row].isBomb) (*grid)[col][row].closeBombs = countNeighboringBombs(*grid, row, col);
		}
	}
}

void revealTiles(Cell (*grid)[COL][ROW], int x, int y)
{
	// Check if the tile coordinates are within the (*grid) bounds
	if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
		return;

	// Check if the tile is already revealed or is a bomb
	if ((*grid)[y][x].isRevealed || (*grid)[y][x].isBomb || (*grid)[y][x].isFlagged)
		return;

	// Reveal this tile
	(*grid)[y][x].isRevealed = TRUE;

	    // If the tile is not blank (has neighboring bombs), stop recursion
	if ((*grid)[y][x].closeBombs > 0)
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

void drawCanvas(SDL_Renderer *renderer, Cell (*grid)[COL][ROW], SDL_Texture *texture, Sprite *sprites, GameState state)
{
	for(int col = 0; col < COL; col++){
		for(int row = 0; row < ROW; row++){
			Cell cell = (*grid)[col][row];

			if(cell.isPressed || cell.isRevealed)							cell.draw.tile = TILE_PRESSED;
			if(!cell.isBomb && cell.isRevealed)								cell.draw.tile = cell.closeBombs;
			if(cell.isFlagged && !cell.isRevealed)							cell.draw.tile = TILE_FLAG;

			if(state == GAME_OVER) {

				if(cell.isBomb && !cell.isRevealed)							cell.draw.tile = BOMB_NORMAL;
				if(cell.isBomb && cell.isFlagged)							cell.draw.tile = TILE_FLAG;
				if(cell.isBomb && cell.isRevealed)							cell.draw.tile = BOMB_RED;
				if(cell.isFlagged && !cell.isBomb)							cell.draw.tile = BOMB_CROSS;
			}

			cell.draw.srcRect.x = sprites[cell.draw.tile].x;
			cell.draw.srcRect.y = sprites[cell.draw.tile].y;

			SDL_RenderCopy(renderer, texture, &cell.draw.srcRect, &cell.draw.destRect );
		}
	}
}

void drawNumbers(SDL_Renderer *renderer, SDL_Texture *texture, int *number_of_bombs, int lastSecond)
{
	#define OFFSET 16

	int bombs = *number_of_bombs;

	int hundreds, tens, ones;
	int s_hundreds, s_tens, s_ones;

	hundreds = (bombs / 100) + OFFSET;
	tens = ((abs(bombs) % 100) / 10) + OFFSET;
	ones = (abs(bombs) % 10) + OFFSET;

	s_hundreds = (lastSecond / 100) + OFFSET;
	s_tens = ((lastSecond % 100) / 10) + OFFSET;
	s_ones = (lastSecond % 10) + OFFSET;

	// 26 minus sign, 27 blank
	if(bombs <= -10 )					hundreds = 26; // MINUS SIGN
	if(bombs < 0 && bombs > -10)		tens = 26;
	if(bombs < 100 && bombs > -10 )		hundreds = 27; // BLANK
	if(bombs < 10 && bombs >= 0)		tens = 27;

	SDL_Rect numbers = { .w = 13, .h = 23, };
	SDL_Rect numbers_destination = { .x = 28, .y = 28, .w = 20, .h = 34, };

	// Left numbers
	numbers.x = sprites[hundreds].x;
	numbers.y = sprites[hundreds].y;
	SDL_RenderCopy(renderer, texture, &numbers, &numbers_destination);

	numbers.x = sprites[tens].x;
	numbers.y = sprites[tens].y;
	numbers_destination.x += 19;
	SDL_RenderCopy(renderer, texture, &numbers, &numbers_destination);

	numbers.x = sprites[ones].x;
	numbers.y = sprites[ones].y;
	numbers_destination.x += 19;
	SDL_RenderCopy(renderer,numbers_texture, &numbers, &numbers_destination);

	// Right numbers
	numbers_destination.x  = 375;
	numbers.x = sprites[s_ones].x;
	numbers.y = sprites[s_ones].y;
	SDL_RenderCopy(renderer, texture, &numbers, &numbers_destination);

	numbers_destination.x -= 19;
	numbers.x = sprites[s_tens].x;
	numbers.y = sprites[s_tens].y;
	SDL_RenderCopy(renderer, texture, &numbers, &numbers_destination);

	numbers_destination.x -= 19;
	numbers.x = sprites[s_hundreds].x;
	numbers.y = sprites[s_hundreds].y;
	SDL_RenderCopy(renderer, texture, &numbers, &numbers_destination);

	#undef OFFSET
}

void drawFace(SDL_Renderer *renderer, SDL_Texture *texture, Rect *face, GameState *state)
{
	face->srcRect.w = 24;
	face->srcRect.h = 24;
	face->destRect.x = 194;
	face->destRect.y = 27;
	face->destRect.w = 36;
	face->destRect.h = 36;

	if (*state == WON)						face->tile = FACE_GLASSES;
	if (*state == GAME_OVER)				face->tile = FACE_DEAD;

	face->srcRect.x = sprites[face->tile].x;
	face->srcRect.y = sprites[face->tile].y;

	SDL_RenderCopy(renderer,texture, &face->srcRect, &face->destRect);
}

void process_input(int *window_open, Cell (*grid)[COL][ROW], Rect *face, GameState *state, int *bomb_count)
{
#define MINE      ((*grid)[gridY][gridX])
#define MINE_LAST ((*grid)[pressed_y][pressed_x])

	SDL_Event event;
	int mouseX, mouseY, gridX, gridY;
	Bool in_canvas, on_face;

	static int pressed_x = 0;
	static int pressed_y = 0;

	while (SDL_PollEvent(&event))
	{
		switch (event.type) {

		case SDL_QUIT:
			*window_open = FALSE;
			break;

		case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					*window_open = FALSE;
				}
				break;

		case SDL_MOUSEBUTTONDOWN:
			mouseX = event.button.x;
			mouseY = event.button.y;

			gridX = (mouseX - CANVAS_X) / CELL_SIZE;
			gridY = (mouseY - CANVAS_Y) / CELL_SIZE;
			pressed_x = gridX;
			pressed_y = gridY;

			in_canvas = ((mouseX >= CANVAS_X && gridX < GRIDSIZE) && (mouseY >= CANVAS_Y && gridY < GRIDSIZE));
			on_face = (mouseX >= 196 && mouseX <= 236 && mouseY >= 26 && mouseY <= 62);

			if( event.button.button == SDL_BUTTON_LEFT)
			{
				if(on_face)
				{
					face->tile = FACE_PRESSED;
					*state = RESTARTING;
				}

				if(in_canvas && *state == PLAYING)
				{
					if(!MINE.isFlagged){
						MINE.isPressed = TRUE;

						face->tile = FACE_SHOCK;

					}
				}

			}
			else if( event.button.button == SDL_BUTTON_RIGHT) {

				if(in_canvas && *state == PLAYING)
				{
					MINE.isPressed = TRUE;
				}

			}

			break;

	case SDL_MOUSEBUTTONUP:
			mouseX = event.button.x;
			mouseY = event.button.y;

			gridX = (mouseX - CANVAS_X) / CELL_SIZE;
			gridY = (mouseY - CANVAS_Y) / CELL_SIZE;

			in_canvas = ((mouseX >= CANVAS_X && gridX < GRIDSIZE) && (mouseY >= CANVAS_Y && gridY < GRIDSIZE));
			on_face = (mouseX >= 196 && mouseX <= 236 && mouseY >= 26 && mouseY <= 62);

				if( event.button.button == SDL_BUTTON_LEFT)
				{
					if (on_face && *state == PLAYING && *state != WON)
					{
						face->tile = FACE_NORMAL;
					}

					if (in_canvas && *state == PLAYING && *state != WON)
					{
						face->tile = FACE_NORMAL;

						if(pressed_y == gridY && pressed_x == gridX && !MINE.isFlagged)
						{
							if (MINE.isBomb) {
								MINE.isRevealed = TRUE;
								MINE.isPressed = FALSE;
								*state = GAME_OVER;
							}


							MINE.isPressed = FALSE;
							revealTiles(grid, gridX, gridY);

						}
						else {
							MINE_LAST.isPressed = FALSE;;
						}
					}
				}
				else if (event.button.button == SDL_BUTTON_RIGHT) {

					if(!MINE.isRevealed && *state == PLAYING)
					{
						if(pressed_y == gridY && pressed_x == gridX) {

							if(!MINE.isFlagged){
								MINE.isFlagged = TRUE;
								MINE.isPressed = FALSE;
								(*bomb_count)--;
							}
							else {
								MINE.isFlagged = FALSE;
								MINE.isPressed = FALSE;
								(*bomb_count)++;
							}
						}
						else {
							MINE_LAST.isPressed = FALSE;
						}
					}
				}

			break;
		}
	}
#undef MINE
#undef MINE_LAST
}

void check_status(Cell (*grid)[COL][ROW], GameState *state)
{
	/* Checking every mine - if it is a bomb, or not revealed - return. If everytile that
	 * is not a bomb is revealed the state is won */
	for(int col = 0; col < COL; col++){
		for(int row = 0; row < ROW; row++){
			if (!(*grid)[col][row].isBomb && !(*grid)[col][row].isRevealed)
				return;
		}
	}
	if (*state == PLAYING) *state = WON;
}

void destroy(void)
{
	SDL_DestroyTexture(background_texture);
	SDL_DestroyTexture(numbers_texture);
	SDL_DestroyTexture(tiles_texture);
	SDL_DestroyTexture(faces_texture);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();
}

int main(void)
{
	Cell grid[COL][ROW];
	GameState state = PLAYING;
	Rect face = { .tile = FACE_NORMAL };

	time_t start, current;
	double elapsed_seconds = 0;
	int last_second = 0;
	time(&start);
	Bool timer_active = TRUE;

	int number_of_bombs = 0;
	int bomb_count = 0;

	int window_open = InitWindowAndIMG(); // Errors handled in function
	initGrid(&grid, &number_of_bombs);
	createWindow();
	loadTextures();
	bomb_count = number_of_bombs;

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	while(window_open)
	{
		check_status(&grid, &state);

		if ( state == GAME_OVER || state == RESTARTING || state == WON )
		{
			timer_active = FALSE;
		}

		if ( timer_active )
		{
			time(&current);
			elapsed_seconds = difftime(current, start);
			if ((int)elapsed_seconds > last_second) {
				last_second = (int)elapsed_seconds;
			}
		}

		if ( state == RESTARTING ) {
			initGrid(&grid, &number_of_bombs);
			state = PLAYING;
			time(&start);
			last_second = -1;
			timer_active = TRUE;
			bomb_count = number_of_bombs;
		}
		/*Create the static background*/
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer,background_texture, NULL, NULL);
		/*Draw the things that is dynamic*/
		drawNumbers(renderer, numbers_texture, &bomb_count, last_second);
		drawFace(renderer, faces_texture, &face, &state);
		drawCanvas(renderer, &grid, tiles_texture, sprites, state);
		/* Present */
		SDL_RenderPresent(renderer);

		process_input(&window_open, &grid, &face, &state, &bomb_count);
	}

	destroy();

	return 0;
}

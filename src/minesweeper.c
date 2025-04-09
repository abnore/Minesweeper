#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "canopy.h"
#include "picasso.h"


#define foreach_cell(body) do {                 \
    for (int col = 0; col < COL; col++) {       \
        for (int row = 0; row < ROW; row++) {   \
            cell *cell = &grid[col][row];       \
           do{ body } while(0);                 \
    }}} while(0);

#define ROW 16
#define COL 16
#define GRIDSIZE 16
#define CELL_SIZE 24
#define TILE_SIZE 16
#define WINDOW_HEIGHT 492
#define WINDOW_WIDTH 426
#define BOMB_CHANCE 6
#define CANVAS_X 21
#define CANVAS_Y 87

typedef enum {
	GAME_OVER,
	PLAYING,
	RESTARTING,
	WON,
} game_state;

typedef struct {
	int x, y;
} sprite;

sprite sprites[] = {
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
} tile_type;

typedef struct {
	picasso_rect src;
	picasso_rect dst;
	tile_type tile;
} rect;

typedef struct {
	bool is_bomb;
	bool is_revealed;
	bool is_flagged;
	bool is_question;
	bool is_pressed;
	rect draw;
	int close_bombs;
} cell;

// API forward declared
void check_status(cell grid[COL][ROW], game_state *state);
void process_input(cell grid[COL][ROW], rect *face, game_state *state, int *bomb_count);
void draw_face(picasso_backbuffer *renderer, picasso_image *texture, rect *face, game_state *state);
void draw_numbers(picasso_backbuffer *renderer, picasso_image *texture, int *number_of_bombs, int last_second);
void draw_canvas(picasso_backbuffer *renderer, cell grid[COL][ROW], picasso_image *texture, sprite *sprites, game_state state);
tile_type select_tile_for_cell(cell *c, game_state state);
void reveal_tiles(cell grid[COL][ROW], int x, int y);
void init_cell(cell *cell, int row, int col);
void init_grid(cell grid[COL][ROW], int *num_bombs);
int count_neighboring_bombs(cell grid[COL][ROW], int x, int y);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if(!init_log(false, true, true)){
        WARN("Log could not initialize");
    }

    canopy_window* window = canopy_create_window("Minesweeper",
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            CANOPY_WINDOW_STYLE_TITLED |
            CANOPY_WINDOW_STYLE_CLOSABLE);
    canopy_set_icon("img/icon.bmp");

    picasso_backbuffer *renderer    = picasso_create_backbuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    picasso_image *background       = picasso_load_bmp("img/Sprites/background.bmp");
    picasso_image *numbers          = picasso_load_bmp("img/Sprites/numbers.bmp");
    picasso_image *tiles            = picasso_load_bmp("img/Sprites/tiles.bmp");
    picasso_image *faces            = picasso_load_bmp("img/Sprites/faces.bmp");

    cell grid[COL][ROW];

    // Initializing the time keeping
    canopy_init_timer();
    canopy_set_fps(24);

    time_t start, current;
    double elapsed_seconds	= 0;
    int last_second			= 0;
    time(&start);
    bool timer_active		= true;

    int number_of_bombs		= 0;
    int bomb_count			= 0;

    init_grid(grid, &number_of_bombs);
    bomb_count				= number_of_bombs;

    // Initial animation state
    game_state state		= PLAYING;
    rect face				= { .tile = FACE_NORMAL };

    //--------------------------------------------------------------------------------------
    // Main Game Loop
    while(!canopy_window_should_close(window))
    {
        // Input
        //----------------------------------------------------------------------------------
        check_status(grid, &state);
        process_input(grid, &face, &state, &bomb_count);

        if( state == GAME_OVER || state == RESTARTING || state == WON )
        {
            timer_active = false;
        }

        if( timer_active )
        {
            time(&current);
            elapsed_seconds = difftime(current, start);
            if( (int)elapsed_seconds > last_second ) last_second = (int)elapsed_seconds;

        }

        if( state == RESTARTING ) {
            init_grid(grid, &number_of_bombs);
            time(&start);

            state			= PLAYING;
            last_second		= -1;
            timer_active	= true;
            bomb_count		= number_of_bombs;
        }

        // Draw
        //----------------------------------------------------------------------------------
        if( canopy_should_render_frame() )
        {
            picasso_clear_backbuffer(renderer);

            /*Create the static background*/
            picasso_blit_rect(renderer,background,
                    (picasso_rect){0,0,
                    background->width, background->height},
                    (picasso_rect){0,0,
                    WINDOW_WIDTH, WINDOW_HEIGHT});

            /*Draw the things that is dynamic*/
            draw_numbers(renderer, numbers, &bomb_count, last_second);
            draw_face(renderer, faces, &face, &state);
            draw_canvas(renderer, grid, tiles, sprites, state);

            /* Present */
            canopy_swap_backbuffer(window, (framebuffer*)renderer);
            canopy_present_buffer(window);
        }
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    picasso_free_image(numbers);
    picasso_free_image(tiles);
    picasso_free_image(faces);
    picasso_free_image(background);
    picasso_destroy_backbuffer(renderer);
    canopy_free_window(window);

    shutdown_log();
    //--------------------------------------------------------------------------------------

    return 0;
}


//--------------------------------------------------------------------------------------
// Implementation of functions
//--------------------------------------------------------------------------------------

void check_status(cell grid[COL][ROW], game_state *state)
{
	/* Checking every mine - if it is a bomb and not revealed - return.
     *
     * If every tile that is not a bomb is revealed the state is won */

    foreach_cell({
        if (!cell->is_bomb && !cell->is_revealed)
        return;
    });

	if (*state == PLAYING) *state = WON;
}

void init_cell(cell *cell, int row, int col)
{
	cell->close_bombs = 0;
	cell->draw.tile = TILE_NORMAL;
	cell->is_flagged = false;
	cell->is_bomb = (arc4random() % BOMB_CHANCE == 0);
	cell->is_revealed = false;
	cell->is_pressed = false;
	cell->is_question = false;
	cell->draw.src.width = TILE_SIZE;
	cell->draw.src.height = TILE_SIZE;
	cell->draw.dst.x = row * CELL_SIZE + CANVAS_X;
	cell->draw.dst.y = col * CELL_SIZE + CANVAS_Y;
	cell->draw.dst.width = CELL_SIZE;
	cell->draw.dst.height = CELL_SIZE;
}

int count_neighboring_bombs(cell grid[COL][ROW], int x, int y)
{
	int bomb_count = 0;

	for( int i = -1; i <= 1; i++ ) {
		for( int j = -1; j <= 1; j++ ){
			int nx = x + i;
			int ny = y + j;

			// Check if neighbor is within grid bounds
			if( nx >= 0 && nx < GRIDSIZE && ny >= 0 && ny < GRIDSIZE ){
				// Check if neighbor is a bomb
				if( grid[ny][nx].is_bomb ) bomb_count++;

			}
		}
	}
	return bomb_count;
}

void init_grid(cell grid[COL][ROW], int *num_bombs)
{
    INFO("Grid initialized");
	*num_bombs = 0;

    foreach_cell({
        init_cell( cell, row, col );
        if( cell->is_bomb == true ) (*num_bombs)++;
    });
    foreach_cell({
        if( !cell->is_bomb )
        cell->close_bombs = count_neighboring_bombs(grid, row, col);
    });

    TRACE("Counted bombs, total amount is %d", *num_bombs);
}

void reveal_tiles(cell grid[COL][ROW], int x, int y)
{
	// Check if the tile coordinates are within the (*grid) bounds
	if( x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE )
		return;

	// Check if the tile is already revealed or is a bomb
	if( grid[y][x].is_revealed || grid[y][x].is_bomb || grid[y][x].is_flagged )
		return;

	// Reveal this tile
	grid[y][x].is_revealed = true;

	    // If the tile is not blank (has neighboring bombs), stop recursion
	if( grid[y][x].close_bombs > 0 )
		return;

	    // Recursively reveal adjacent tiles
	for( int i = -1; i <= 1; i++ ){
		for( int j = -1; j <= 1; j++ ){
            // Avoid the current tile itself
			if( i != 0 || j != 0 ) reveal_tiles(grid, x + i, y + j);
		}
	}
}

tile_type select_tile_for_cell(cell *c, game_state state)
{
    // --- Game over: bombs and mistakes ---
    if (state == GAME_OVER) {
        if (c->is_bomb && c->is_revealed)   return BOMB_RED;
        if (c->is_bomb && !c->is_flagged)   return BOMB_NORMAL;
        if (c->is_flagged && !c->is_bomb)   return BOMB_CROSS;
        if (c->is_flagged)                  return TILE_FLAG;
    }

    // --- Flags and question marks ---
    if (c->is_flagged && !c->is_revealed)
        return TILE_FLAG;

    if (c->is_question && !c->is_revealed && c->is_pressed)
        return TILE_QUESTION_PRESSED;

    if (c->is_question && !c->is_revealed)
        return TILE_QUESTION;

    // --- Revealed tile: show number ---
    if (c->is_revealed && !c->is_bomb)
        return c->close_bombs;

    // --- Pressed tile (but not revealed) ---
    if (c->is_pressed)
        return TILE_PRESSED;

    // --- Default: hidden tile ---
    return TILE_NORMAL;
}

void draw_canvas(picasso_backbuffer *renderer, cell grid[COL][ROW], picasso_image *texture, sprite *sprites, game_state state)
{
    foreach_cell({
        cell->draw.tile = select_tile_for_cell(cell, state);

        cell->draw.src.x = sprites[cell->draw.tile].x;
        cell->draw.src.y = sprites[cell->draw.tile].y;

        picasso_blit_rect(renderer, texture, cell->draw.src, cell->draw.dst);
    });
}

void draw_numbers(picasso_backbuffer *renderer, picasso_image *texture, int *number_of_bombs, int last_second)
{
	#define OFFSET 16

	int bombs			= *number_of_bombs;

	int	hundreds		= (bombs / 100)				 + OFFSET;
	int tens			= ((abs(bombs) % 100) / 10)  + OFFSET;
	int ones			= (abs(bombs) % 10)			 + OFFSET;

	int s_hundreds		= (last_second / 100)		 + OFFSET;
	int s_tens			= ((last_second % 100) / 10) + OFFSET;
	int s_ones			= (last_second % 10)		 + OFFSET;

	// 26 minus sign, 27 blank
	if(bombs <= -10 )	        		hundreds = 26; // MINUS SIGN
	if(bombs < 0 && bombs > -10)		tens = 26;
	if(bombs < 100 && bombs > -10 )		hundreds = 27; // BLANK
	if(bombs < 10 && bombs >= 0)		tens = 27;

	picasso_rect numbers					= { .width = 13, .height = 23, };
	picasso_rect numbers_destination		= { 28, 28, 20, 34 };

	// Left numbers
	numbers.x = sprites[hundreds].x;
	numbers.y = sprites[hundreds].y;
	numbers_destination.x += 0;
	picasso_blit_rect(renderer, texture, numbers, numbers_destination);

	numbers.x = sprites[tens].x;
	numbers.y = sprites[tens].y;
	numbers_destination.x += 19;
	picasso_blit_rect(renderer, texture, numbers, numbers_destination);

	numbers.x = sprites[ones].x;
	numbers.y = sprites[ones].y;
	numbers_destination.x += 19;
	picasso_blit_rect(renderer, texture, numbers, numbers_destination);

	// Right numbers
	numbers.x = sprites[s_ones].x;
	numbers.y = sprites[s_ones].y;
    numbers_destination.x  = 375;
	picasso_blit_rect(renderer, texture, numbers, numbers_destination);

	numbers.x = sprites[s_tens].x;
	numbers.y = sprites[s_tens].y;
    numbers_destination.x -= 19;
	picasso_blit_rect(renderer, texture, numbers, numbers_destination);

	numbers.x = sprites[s_hundreds].x;
	numbers.y = sprites[s_hundreds].y;
    numbers_destination.x -= 19;
	picasso_blit_rect(renderer, texture, numbers, numbers_destination);

	#undef OFFSET
}

void draw_face(picasso_backbuffer *renderer, picasso_image *texture, rect *face, game_state *state)
{
	face->src.width	    = 24;
	face->src.height    = 24;
	face->dst.x	        = 194;
	face->dst.y 	    = 27;
	face->dst.width 	= 36;
	face->dst.height	= 36;

	if (*state == WON)  		face->tile = FACE_GLASSES;
	if (*state == GAME_OVER)	face->tile = FACE_DEAD;

	face->src.x = sprites[face->tile].x;
	face->src.y = sprites[face->tile].y;

	picasso_blit_rect(renderer,texture, face->src, face->dst);
}

void process_input(cell grid[COL][ROW], rect *face, game_state *state, int *bomb_count)
{
#define MINE      (grid[grid_y][grid_x])
#define MINE_LAST (grid[pressed_y][pressed_x])

    canopy_event event;
    int mouse_x, mouse_y, grid_x, grid_y;
    bool in_canvas, on_face;

    static int pressed_x = -1;
    static int pressed_y = -1;

    while (canopy_poll_event(&event)) {
        switch (event.type) {

        case CANOPY_EVENT_KEY:
            if (event.key.action == CANOPY_KEY_PRESS &&
                event.key.keycode == CANOPY_KEY_ESCAPE) {
                INFO("Escape pressed");
            }
            break;

        case CANOPY_EVENT_MOUSE:
            switch (event.mouse.action) {

            case CANOPY_MOUSE_PRESS:
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;

                grid_x = (mouse_x - CANVAS_X) / CELL_SIZE;
                grid_y = (mouse_y - CANVAS_Y) / CELL_SIZE;

                pressed_x = grid_x;
                pressed_y = grid_y;

                in_canvas = (mouse_x >= CANVAS_X && grid_x < GRIDSIZE &&
                             mouse_y >= CANVAS_Y && grid_y < GRIDSIZE);

                on_face = (mouse_x >= 196 && mouse_x <= 236 &&
                           mouse_y >= 26 && mouse_y <= 62);

                switch (event.mouse.button) {
                    case CANOPY_MOUSE_BUTTON_LEFT:
                        if (on_face) {
                            face->tile = FACE_PRESSED;
                            *state = RESTARTING;
                        } else if (in_canvas && *state == PLAYING && !MINE.is_flagged) {
                            MINE.is_pressed = true;
                            face->tile = FACE_SHOCK;
                        }
                        break;

                    case CANOPY_MOUSE_BUTTON_RIGHT:
                        if (in_canvas && *state == PLAYING) {
                            MINE.is_pressed = true;
                        }
                        break;

                    default: break;
                }
                break;

            case CANOPY_MOUSE_RELEASE:
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;

                grid_x = (mouse_x - CANVAS_X) / CELL_SIZE;
                grid_y = (mouse_y - CANVAS_Y) / CELL_SIZE;

                in_canvas = (mouse_x >= CANVAS_X && grid_x < GRIDSIZE &&
                             mouse_y >= CANVAS_Y && grid_y < GRIDSIZE);

                // Always unpress the previously pressed tile
                MINE_LAST.is_pressed = false;

                // Only process if release matches press and is in bounds
                if (pressed_x != grid_x || pressed_y != grid_y || !in_canvas)
                    break;

                switch (event.mouse.button) {
                    case CANOPY_MOUSE_BUTTON_LEFT:
                        if (*state == PLAYING && *state != WON) {
                            face->tile = FACE_NORMAL;

                            if (!MINE_LAST.is_flagged) {
                                if (MINE_LAST.is_bomb) {
                                    MINE_LAST.is_revealed = true;
                                    *state = GAME_OVER;
                                }

                                reveal_tiles(grid, grid_x, grid_y);

                                if (MINE_LAST.is_question)
                                    MINE_LAST.is_question = false;
                            }
                        }
                        break;

                    case CANOPY_MOUSE_BUTTON_RIGHT:
                        if (!MINE_LAST.is_revealed && *state == PLAYING) {
                            if (!MINE_LAST.is_flagged && !MINE_LAST.is_question) {
                                MINE_LAST.is_flagged = true;
                                (*bomb_count)--;
                            }
                            else if (MINE_LAST.is_flagged) {
                                MINE_LAST.is_flagged = false;
                                MINE_LAST.is_question = true;
                                (*bomb_count)++;
                            }
                            else {
                                MINE_LAST.is_question = false;
                            }
                        }
                        break;

                    default: break;
                }

                pressed_x = -1;
                pressed_y = -1;
                break;

            default: break;
            } // end switch (event.mouse.action)
            break;

        default: break;
        } // end switch (event.type)
    } // end while (canopy_poll_event)

#undef MINE
#undef MINE_LAST
}

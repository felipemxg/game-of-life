#include <SDL.h>
#include <stdlib.h>
#include <time.h>

#define GRID_W 80
#define GRID_H 60

struct CELL
{
    char state;
    char next_state;
    char sides;
} **cell_grid;

/* generate grid */
struct CELL** create_grid(int w, int h)
{
    struct CELL **grid;
    grid = malloc(h * sizeof(struct CELL *));

    srand(time(NULL));

    int x,y;
    for (y = 0; y < h; y++)
    {
        grid[y] = malloc(w * sizeof(struct CELL));

        for (x = 0; x < w; x++)
        {
            int shot = rand() % 101;
            if (shot > 90)
                grid[y][x].state = 1;
            else
                grid[y][x].state = 0;
            grid[y][x].sides = 0;
        }
    }

    return grid;
}

/* control refresh time */
int tick(int t)
{
    static int start_ticks = 0;
    int get_ticks = SDL_GetTicks();
    if (get_ticks - start_ticks > t)
    {
        start_ticks = get_ticks;
        return 1;
    }
    return 0;
}

#define EDIT  0
#define GROW  1
#define CLEAN 2
#define NEW   3

int main(int argc, char *argv[])
{
    /* init SDL and create window */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("Conway's Game of Life", NULL);
    SDL_Surface *screen = SDL_SetVideoMode(800, 600, 32, SDL_DOUBLEBUF | SDL_HWSURFACE);

    /* SDL event handler */
    SDL_Event event;

    cell_grid = create_grid(GRID_W, GRID_H);

    SDL_Surface *cell = SDL_LoadBMP("cell.bmp");

    int delay = 1000;
    if (argc > 1)
        sscanf(argv[1], "%d", &delay);

    /* game and keyboard state */
    int game_state = GROW;
    Uint8 *keystate = SDL_GetKeyState(NULL);
    /* cursor coordinates */
    int mouse_x, mouse_y;

    /* main loop */
    int quit = 0;
    while (!quit)
    {
        /* handle events */
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = 1;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                quit = 1;
        }

        /* change game state */
        if (keystate[SDLK_e])
            game_state = EDIT;
        else if (keystate[SDLK_s])
            game_state = GROW;
        else if (keystate[SDLK_c])
            game_state = CLEAN;
        else if (keystate[SDLK_n])
            game_state = NEW;
        else if (keystate[SDLK_x])
            game_state = -1;

        /* grow state */
        if (game_state == GROW)
        {
            if (tick(delay))
            {
                SDL_WM_SetCaption("Conway's Game of Life - Grow mode", NULL);

                int x, y;
                /* grid loop */
                for (y = 0; y < GRID_H; y++)
                {
                    for (x = 0; x < GRID_W; x++)
                    {
                        cell_grid[y][x].sides = 0;

                        /* top */
                        if (y > 0 && cell_grid[y - 1][x].state)
                            cell_grid[y][x].sides++;

                        /* bottom */
                        if (y < GRID_H - 1 && cell_grid[y + 1][x].state)
                            cell_grid[y][x].sides++;

                        /* left */
                        if (x > 0 && cell_grid[y][x - 1].state)
                            cell_grid[y][x].sides++;

                        /* right */
                        if (x < GRID_W - 1 && cell_grid[y][x + 1].state)
                            cell_grid[y][x].sides++;

                        /* top-left */
                        if (y > 0 && x > 0 && cell_grid[y - 1][x - 1].state)
                            cell_grid[y][x].sides++;

                        /* top-right */
                        if (y > 0 && x < GRID_W - 1 && cell_grid[y - 1][x + 1].state)
                            cell_grid[y][x].sides++;

                        /* bottom-left */
                        if (y < GRID_H - 1 && x > 0 && cell_grid[y + 1][x - 1].state)
                            cell_grid[y][x].sides++;

                        /* bottom-right */
                        if (y < GRID_H - 1 && x < GRID_W - 1 && cell_grid[y + 1][x + 1].state)
                            cell_grid[y][x].sides++;

                        /* game logics */
                        if (cell_grid[y][x].sides < 2)
                            cell_grid[y][x].next_state = 0;
                        else if (cell_grid[y][x].sides > 3)
                            cell_grid[y][x].next_state = 0;
                        else if (cell_grid[y][x].sides == 3)
                            cell_grid[y][x].next_state = 1;
                        else
                            cell_grid[y][x].next_state = cell_grid[y][x].state;
                    }
                }

                for (y = 0; y < GRID_H; y++)
                {
                    for (x = 0; x < GRID_W; x++)
                    {
                        cell_grid[y][x].state = cell_grid[y][x].next_state;

                        SDL_Rect rect = {x*10, y*10};

                        /* if cell is alive, draw her*/
                        if (cell_grid[y][x].state)
                            SDL_BlitSurface(cell, NULL, screen, &rect);
                    }
                }

                SDL_Flip(screen);
                SDL_FillRect(screen, NULL, 0xFFFFFF);
            }
        }

        /* game edit mode */
        else if (game_state == EDIT)
        {
            SDL_WM_SetCaption("Conway's Game of Life - Edit mode", NULL);

            /* grid loop */
            int x, y;
            for (y = 0; y < GRID_H; y++)
            {
                for (x = 0; x < GRID_W; x++)
                {
                    SDL_Rect rect = {x*10, y*10};

                    /* if cell is alive, draw her */
                    if (cell_grid[y][x].state)
                        SDL_BlitSurface(cell, NULL, screen, &rect);

                    /* get mouse coordinates */
                    SDL_GetMouseState(&mouse_x, &mouse_y);

                    if (mouse_y >= y*10 && mouse_y < (y+1)*10 &&
                        mouse_x >= x*10 && mouse_x < (x+1)*10)
                    {
                        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1))
                            cell_grid[y][x].state = 1;
                        else if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(3))
                            cell_grid[y][x].state = 0;
                    }
                }
            }

            SDL_Flip(screen);
            SDL_FillRect(screen, NULL, 0xFFFFFF);
        }

        /* clear grid */
        else if (game_state == CLEAN)
        {
            SDL_WM_SetCaption("Conway's Game of Life - Clean mode", NULL);

            /* grid loop */
            int x, y;
            for (y = 0; y < GRID_H; y++)
            {
                for (x = 0; x < GRID_W; x++)
                {
                    SDL_Rect rect = {x*10, y*10};

                    /* if cell is alive, draw her*/
                    if (cell_grid[y][x].state)
                        SDL_BlitSurface(cell, NULL, screen, &rect);

                    cell_grid[y][x].state = 0;
                    cell_grid[y][x].sides = 0;
                    cell_grid[y][x].next_state = 0;
                }
            }

            SDL_Flip(screen);
            SDL_FillRect(screen, NULL, 0xFFFFFF);

            game_state = EDIT;
        }

        /* new random grid */
        else if (game_state == NEW)
        {
            int x,y;
            for (y = 0; y < GRID_H; y++)
            {
                for (x = 0; x < GRID_W; x++)
                {
                    int shot = rand() % 101;
                    if (shot > 50)
                        cell_grid[y][x].state = 1;
                    else
                        cell_grid[y][x].state = 0;
                    cell_grid[y][x].sides = 0;
                }
            }

            game_state = GROW;
        }
        else
            SDL_WM_SetCaption("Conway's Game of Life - Stop mode", NULL);

        SDL_Delay(1000 / 60);
    }

    SDL_Quit();
    return 0;
}

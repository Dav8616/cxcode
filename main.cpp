#include <os.h>
#include <libndls.h>
#include <SDL/SDL.h>
#include "editor.h"
#include "font.h"
#include "mouse.h"

SDL_Surface *screen = NULL;
Editor ed;

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 1;
    }

    screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
    if (!screen)
    {
        SDL_Quit();
        return 1;
    }

    editor_init(&ed);

    if (argc > 1)
    {
        load_file(&ed, argv[1]);
    }

    int running = 1;
    SDL_Event event;
    int prev_click = 0;

    editor_draw(&ed);

    while (running)
    {
        editor_draw(&ed);
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                SDLKey key = event.key.keysym.sym;
                int ctrl = event.key.keysym.mod & KMOD_CTRL;

                // Check for exit
                if (key == SDLK_ESCAPE && ed.mode == MODE_EDIT)
                {
                    running = 0;
                }
                else
                {
                    editor_handle_key(&ed, key, ctrl);
                }
                mouse_update(&ed.mouse);
            }
        }
        mouse_update(&ed.mouse); // updates ed.mouse.x/y/down/click

        if (ed.mouse.click && !prev_click)
        {
            // Call the new centralized handler function from editor.cpp
            editor_handle_click(&ed, ed.mouse.x, ed.mouse.y);
        }

        // Store the current click state for the next iteration
        prev_click = ed.mouse.click;

        SDL_Delay(20);
    }

    editor_free(&ed);
    SDL_Quit();
    return 0;
}
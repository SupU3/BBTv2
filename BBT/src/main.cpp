#include <iostream>
#include "SDL3/SDL.h"

int main()
{

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window;

    window = SDL_CreateWindow("MORDI",600,400,0);

    bool runing = true;

    while (runing)
    {

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {

            if (event.type == SDL_EVENT_QUIT)
            {

                runing = false;

            }

        }



    }

    printf("Pichita");

    return 0;

}
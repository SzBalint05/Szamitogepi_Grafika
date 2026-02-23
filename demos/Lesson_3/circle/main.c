#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "circle.h"

#define WIDTH 800
#define HEIGHT 600

int SDL_main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Circle Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, 0);

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    srand(time(NULL));

    Circle circles[MAX_CIRCLE_COUNT];
    int circleCount = 0;

    ApproxMode mode = APPROX_BY_STEPS;
    double param = 40;

    bool running = true;
    bool creating = false;
    Circle* dragged = NULL;

    int startX, startY;

    SDL_Event e;

    while (running) {

        while (SDL_PollEvent(&e)) {

            if (e.type == SDL_QUIT)
                running = false;

            if (e.type == SDL_MOUSEBUTTONDOWN) {

                int mx = e.button.x;
                int my = e.button.y;

                for (int i = 0; i < circleCount; ++i) {
                    if (point_in_circle(&circles[i], mx, my)) {
                        dragged = &circles[i];
                        break;
                    }
                }

                if (!dragged && circleCount < MAX_CIRCLE_COUNT) {
                    creating = true;
                    startX = mx;
                    startY = my;
                }
            }

            if (e.type == SDL_MOUSEBUTTONUP) {

                if (creating && circleCount < MAX_CIRCLE_COUNT) {

                    int dx = e.button.x - startX;
                    int dy = e.button.y - startY;
                    double r = sqrt(dx*dx + dy*dy);

                    Color randomColor = {
                        rand()%256,
                        rand()%256,
                        rand()%256
                    };

                    set_circle_data(&circles[circleCount],
                                    startX, startY, r,
                                    randomColor);

                    circleCount++;
                }

                creating = false;
                dragged = NULL;
            }

            if (e.type == SDL_MOUSEMOTION) {

                int mx = e.motion.x;
                int my = e.motion.y;

                if (dragged) {
                    dragged->x = mx;
                    dragged->y = my;
                }

                for (int i = 0; i < circleCount; ++i) {
                    circles[i].selected =
                        point_in_circle(&circles[i], mx, my);
                }
            }

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_1) { mode = APPROX_BY_STEPS; param = 40; }
                if (e.key.keysym.sym == SDLK_2) { mode = APPROX_BY_ANGLE; param = M_PI/20; }
                if (e.key.keysym.sym == SDLK_3) { mode = APPROX_BY_MAXLEN; param = 10; }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < circleCount; ++i) {
            draw_circle(renderer, &circles[i], mode, param);

            if (circles[i].selected) {
                SDL_SetRenderDrawColor(renderer,255,255,255,255);
                SDL_RenderDrawLine(renderer,
                    circles[i].x-6, circles[i].y,
                    circles[i].x+6, circles[i].y);
                SDL_RenderDrawLine(renderer,
                    circles[i].x, circles[i].y-6,
                    circles[i].x, circles[i].y+6);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
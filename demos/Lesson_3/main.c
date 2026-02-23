#include <stdio.h>
#include <stdbool.h>
#include "line.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main(int argc, char *argv[]) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Szakasz rajzolo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Line lines[MAX_LINE_COUNT];
    int lineCount = 0;

    Color palette[PALETTE_SIZE] = {
        {255,0,0},
        {0,255,0},
        {0,0,255},
        {255,255,0},
        {255,0,255},
        {0,255,255}
    };

    int selectedColor = 0;
    bool running = true;
    bool firstClick = false;
    bool rectangleMode = false;
    int startX, startY;

    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_MOUSEMOTION) {
                printf("Eger pozicio: %d %d\n", event.motion.x, event.motion.y);
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                int x = event.button.x;
                int y = event.button.y;

                if (y < PALETTE_HEIGHT) {
                    int index = x / (WINDOW_WIDTH / PALETTE_SIZE);
                    if (index < PALETTE_SIZE)
                        selectedColor = index;
                } else {

                    if (!firstClick) {
                        startX = x;
                        startY = y;
                        firstClick = true;
                    } else {

                        if (lineCount < MAX_LINE_COUNT) {
                            lines[lineCount].x1 = startX;
                            lines[lineCount].y1 = startY;
                            lines[lineCount].x2 = x;
                            lines[lineCount].y2 = y;
                            lines[lineCount].color = palette[selectedColor];
                            lineCount++;
                        }

                        firstClick = false;
                    }
                }
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_r)
                    rectangleMode = !rectangleMode;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawPalette(renderer, palette, selectedColor, WINDOW_WIDTH);

        for (int i = 0; i < lineCount; i++) {
            if (!rectangleMode)
                drawLine(renderer, &lines[i]);
            else
                drawFilledRect(renderer, &lines[i]);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
#ifndef LINE_H
#define LINE_H

#include <SDL2/SDL.h>

#define MAX_LINE_COUNT 100
#define PALETTE_SIZE 6
#define PALETTE_HEIGHT 60

typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} Color;

typedef struct {
    int x1, y1;
    int x2, y2;
    Color color;
} Line;

void drawLine(SDL_Renderer *renderer, const Line *line);
void drawFilledRect(SDL_Renderer *renderer, const Line *rect);
void drawPalette(SDL_Renderer *renderer, Color palette[], int selected, int windowWidth);

#endif
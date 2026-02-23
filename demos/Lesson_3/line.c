#include "line.h"

void drawLine(SDL_Renderer *renderer, const Line *line) {
    SDL_SetRenderDrawColor(renderer, line->color.r, line->color.g, line->color.b, 255);
    SDL_RenderDrawLine(renderer, line->x1, line->y1, line->x2, line->y2);
}

void drawFilledRect(SDL_Renderer *renderer, const Line *rect) {
    SDL_Rect r;

    r.x = rect->x1;
    r.y = rect->y1;
    r.w = rect->x2 - rect->x1;
    r.h = rect->y2 - rect->y1;

    SDL_SetRenderDrawColor(renderer, rect->color.r, rect->color.g, rect->color.b, 255);
    SDL_RenderFillRect(renderer, &r);
}

void drawPalette(SDL_Renderer *renderer, Color palette[], int selected, int windowWidth) {
    int width = windowWidth / PALETTE_SIZE;

    for (int i = 0; i < PALETTE_SIZE; i++) {
        SDL_Rect rect = { i * width, 0, width, PALETTE_HEIGHT };

        SDL_SetRenderDrawColor(renderer, palette[i].r, palette[i].g, palette[i].b, 255);

        SDL_RenderFillRect(renderer, &rect);

        if (i == selected) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
}
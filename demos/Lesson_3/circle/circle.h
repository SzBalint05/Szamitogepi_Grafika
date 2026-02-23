#ifndef CIRCLE_H
#define CIRCLE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_CIRCLE_COUNT 50

typedef struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} Color;

typedef struct Circle
{
    double x;
    double y;
    double radius;
    Color color;
    bool selected;
} Circle;

typedef enum {
    APPROX_BY_STEPS,
    APPROX_BY_ANGLE,
    APPROX_BY_MAXLEN
} ApproxMode;

void set_circle_data(Circle* circle, double x, double y, double radius, Color color);
double calc_circle_area(const Circle* circle);

void draw_circle(SDL_Renderer* renderer,
                 const Circle* circle,
                 ApproxMode mode,
                 double param);

bool point_in_circle(const Circle* circle, int px, int py);

#endif
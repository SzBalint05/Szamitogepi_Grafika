#define _USE_MATH_DEFINES
#include <math.h>
#include "circle.h"

void set_circle_data(Circle* circle, double x, double y, double radius, Color color)
{
    circle->x = x;
    circle->y = y;

    if (radius > 0.0)
        circle->radius = radius;
    else
        circle->radius = NAN;

    circle->color = color;
    circle->selected = false;
}

double calc_circle_area(const Circle* circle)
{
    return circle->radius * circle->radius * M_PI;
}

static void draw_line(SDL_Renderer* r,
                      int x1, int y1,
                      int x2, int y2,
                      Color c)
{
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, 255);
    SDL_RenderDrawLine(r, x1, y1, x2, y2);
}

void draw_circle(SDL_Renderer* renderer,
                 const Circle* circle,
                 ApproxMode mode,
                 double param)
{
    int steps;

    if (mode == APPROX_BY_STEPS) {
        steps = (int)param;
    }
    else if (mode == APPROX_BY_ANGLE) {
        steps = (int)(2 * M_PI / param);
    }
    else {
        double kerulet = 2 * M_PI * circle->radius;
        steps = (int)(kerulet / param);
    }

    if (steps < 6) steps = 6;

    double angleStep = 2 * M_PI / steps;

    for (int i = 0; i < steps; ++i) {

        double a1 = i * angleStep;
        double a2 = (i + 1) * angleStep;

        int x1 = circle->x + circle->radius * cos(a1);
        int y1 = circle->y + circle->radius * sin(a1);

        int x2 = circle->x + circle->radius * cos(a2);
        int y2 = circle->y + circle->radius * sin(a2);

        draw_line(renderer, x1, y1, x2, y2, circle->color);
    }
}

bool point_in_circle(const Circle* circle, int px, int py)
{
    double dx = px - circle->x;
    double dy = py - circle->y;
    return dx*dx + dy*dy <= circle->radius * circle->radius;
}
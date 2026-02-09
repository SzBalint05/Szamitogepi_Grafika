#ifndef BRICK_H
#define BRICK_H

typedef struct Brick
{
    double a;
    double b;
    double c;
} Brick;

void set_size(Brick* brick, double a, double b, double c);

double calc_volume(const Brick* brick);

double calc_surface(const Brick* brick);

int has_square_face(const Brick* brick);

#endif

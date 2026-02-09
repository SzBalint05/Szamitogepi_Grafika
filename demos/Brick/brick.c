#include "brick.h"
#include <math.h>
void set_size(Brick* brick, double a, double b, double c)
{
    if (a > 0.0 && b > 0.0 && c > 0.0) {
        brick->a = a;
        brick->b = b;
        brick->c = c;
    } else {
        brick->a = NAN;
        brick->b = NAN;
        brick->c = NAN;
    }
}

double calc_volume(const Brick* brick)
{
    return brick->a * brick->b * brick->c;
}

double calc_surface(const Brick* brick)
{
    return 2.0 * (brick->a * brick->b + brick->a * brick->c + brick->b * brick->c);
}

int has_square_face(const Brick* brick)
{
    if (brick->a == brick->b || brick->a == brick->c || brick->b == brick->c) {
        return 1;
    }

    return 0;
}

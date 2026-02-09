#include "brick.h"

#include <stdio.h>

int main(void)
{
    Brick brick;
    double volume;
    double surface;

    set_size(&brick, 4, 4, 6);

    volume = calc_volume(&brick);
    surface = calc_surface(&brick);

    printf("Volume: %lf\n", volume);
    printf("Surface: %lf\n", surface);

    if (has_square_face(&brick)) {
        printf("The brick has a square face.\n");
    } else {
        printf("The brick does not have a square face.\n");
    }

    return 0;
}
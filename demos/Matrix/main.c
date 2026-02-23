#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "matrix.h"

int main() {
    double matrix[SIZE][SIZE];
    init_identity_matrix(matrix);

    printf("Egységmátrix:\n");
    print_matrix(matrix);

    multiply_matrix_by_scalar(matrix, 2.0);
    printf("Skalárral szorzva (2x):\n");
    print_matrix(matrix);

    init_identity_matrix(matrix);

    scale(matrix, 2.0, 3.0);
    shift(matrix, 5.0, -2.0);
    rotate(matrix, M_PI / 4.0);

    printf("Összetett transzformáció:\n");
    print_matrix(matrix);

    double x = 1.0, y = 1.0;
    double new_x, new_y;

    transform_point(matrix, x, y, &new_x, &new_y);

    printf("Eredeti pont: (%.3f, %.3f)\n", x, y);
    printf("Transzformált pont: (%.3f, %.3f)\n", new_x, new_y);

    return 0;
}
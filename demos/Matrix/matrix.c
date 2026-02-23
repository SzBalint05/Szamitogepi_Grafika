#include <stdio.h>
#include <math.h>
#include "matrix.h"

void init_identity_matrix(double m[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            m[i][j] = (i == j) ? 1.0 : 0.0;
}

void multiply_matrix_by_scalar(double m[SIZE][SIZE], double scalar) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            m[i][j] *= scalar;
}

void multiply_matrices(double result[SIZE][SIZE], double a[SIZE][SIZE], double b[SIZE][SIZE]) {

    double temp[SIZE][SIZE];

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            temp[i][j] = 0.0;
            for (int k = 0; k < SIZE; k++)
                temp[i][j] += a[i][k] * b[k][j];
        }
    }

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            result[i][j] = temp[i][j];
}

void transform_point(double matrix[SIZE][SIZE], double x, double y, double* out_x, double* out_y) {

    double point[SIZE] = { x, y, 1.0 };
    double result[SIZE] = { 0 };

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            result[i] += matrix[i][j] * point[j];

    *out_x = result[0] / result[2];
    *out_y = result[1] / result[2];
}

void scale(double matrix[SIZE][SIZE], double sx, double sy) {
    double s[SIZE][SIZE];
    init_identity_matrix(s);

    s[0][0] = sx;
    s[1][1] = sy;

    multiply_matrices(matrix, matrix, s);
}

void shift(double matrix[SIZE][SIZE], double tx, double ty) {
    double t[SIZE][SIZE];
    init_identity_matrix(t);

    t[0][2] = tx;
    t[1][2] = ty;

    multiply_matrices(matrix, matrix, t);
}

void rotate(double matrix[SIZE][SIZE], double angle_rad) {
    double r[SIZE][SIZE];
    init_identity_matrix(r);

    r[0][0] = cos(angle_rad);
    r[0][1] = -sin(angle_rad);
    r[1][0] = sin(angle_rad);
    r[1][1] = cos(angle_rad);

    multiply_matrices(matrix, matrix, r);
}

void print_matrix(double m[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++)
            printf("%8.3f ", m[i][j]);
        printf("\n");
    }
    printf("\n");
}
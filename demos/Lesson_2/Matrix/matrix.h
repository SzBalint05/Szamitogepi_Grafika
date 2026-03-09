#ifndef MATRIX_H
#define MATRIX_H

#define SIZE 3

void init_identity_matrix(double m[SIZE][SIZE]);
void multiply_matrix_by_scalar(double m[SIZE][SIZE], double scalar);
void multiply_matrices(double result[SIZE][SIZE], double a[SIZE][SIZE], double b[SIZE][SIZE]);

void transform_point(double matrix[SIZE][SIZE], double x, double y, double* out_x, double* out_y);

void scale(double matrix[SIZE][SIZE], double sx, double sy);
void shift(double matrix[SIZE][SIZE], double tx, double ty);
void rotate(double matrix[SIZE][SIZE], double angle_rad);

void print_matrix(double m[SIZE][SIZE]);

#endif
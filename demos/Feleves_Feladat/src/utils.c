#include "utils.h"
#include <stdlib.h>
#include <math.h>
#define _USE_MATH_DEFINES

double degree_to_radian(double degree){
    return degree * M_PI / 180.0;
}

float random_float(float min, float max){
    return min + ((float)rand() / RAND_MAX) * (max - min);
}
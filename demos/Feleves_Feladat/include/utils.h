#ifndef UTILS_H
#define UTILS_H

#include <GL/gl.h>

typedef struct{
    float x;
    float y;
    float z;
}vec3;

typedef struct Color{
    float red;
    float green;
    float blue;
}Color;

typedef struct Material{
    Color ambient;
    Color diffuse;
    Color specular;
    float shininess;
}Material;

double degree_to_radian(double degree);
float random_float(float min, float max);
void draw_bar(float x, float y, float width, float height, float percentage, Color bg, Color fg);
void draw_fullscreen_overlay(int w, int h, GLuint texture);

#endif // UTILS_H
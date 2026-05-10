#include "utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define _USE_MATH_DEFINES

double degree_to_radian(double degree){
    return degree * M_PI / 180.0;
}

float random_float(float min, float max){
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

void draw_bar(float x, float y, float width, float height, float percentage, Color bg, Color fg){
    glColor3f(bg.red, bg.green, bg.blue);
    glBegin(GL_QUADS);
        glVertex2f(x, y); glVertex2f(x + width, y);
        glVertex2f(x + width, y + height); glVertex2f(x, y + height);
    glEnd();
    
    glColor3f(fg.red, fg.green, fg.blue);
    float fill_w = percentage * width;
    glBegin(GL_QUADS);
        glVertex2f(x, y); glVertex2f(x + fill_w, y);
        glVertex2f(x + fill_w, y + height); glVertex2f(x, y + height);
    glEnd();
}

void draw_fullscreen_overlay(int w, int h, GLuint texture){
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f); glVertex2f(w, 0.0f); 
        glVertex2f(w, h); glVertex2f(0.0f, h);
    glEnd();
    glDisable(GL_BLEND);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(1.0f, 1.0f, 1.0f);

    float img_w = w * 0.5f;
    float img_h = img_w * 0.75f;
    float img_x = (w - img_w) / 2.0f;
    float img_y = (h - img_h) / 2.0f;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(img_x, img_y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(img_x + img_w, img_y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(img_x + img_w, img_y + img_h);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(img_x, img_y + img_h);
    glEnd();
}
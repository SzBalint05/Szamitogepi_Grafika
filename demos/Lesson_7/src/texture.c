#include "texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>

GLuint load_texture(char* filename)
{
    SDL_Surface* surface;
    GLuint texture_name;

    surface = IMG_Load(filename);

    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, (Pixel*)(surface->pixels));
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    SDL_FreeSurface(surface);
    return texture_name;
}

GLuint generate_checkerboard_texture()
{
    GLuint texture_name;
    Pixel image[256][256];
    int c;

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            c = ((((i & 0x20) == 0) ^ ((j & 0x20) == 0))) * 255;
            image[i][j][0] = (GLubyte)c;
            image[i][j][1] = (GLubyte)c;
            image[i][j][2] = (GLubyte)c;
        }
    }

    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture_name;
}

GLuint generate_circle_texture()
{
    GLuint texture_name;
    Pixel image[256][256];

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            float dx = i - 128.0f;
            float dy = j - 128.0f;
            if (sqrt(dx * dx + dy * dy) < 100.0f) {
                image[i][j][0] = 255; image[i][j][1] = 0; image[i][j][2] = 0;
            } else {
                image[i][j][0] = 255; image[i][j][1] = 255; image[i][j][2] = 255;
            }
        }
    }

    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture_name;
}

GLuint generate_gradient_texture()
{
    GLuint texture_name;
    Pixel image[256][256];

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            image[i][j][0] = i;
            image[i][j][1] = j;
            image[i][j][2] = 128;
        }
    }

    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture_name;
}
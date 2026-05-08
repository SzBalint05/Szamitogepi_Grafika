#include "texture.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

GLuint load_texture(const char* filename)
{
    SDL_Surface* raw_surface = IMG_Load(filename);
    if (!raw_surface){
        printf("Can't find texture: %s\n", filename);
        return 0;
    }

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(raw_surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(raw_surface);

    GLuint texture_name;
    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    SDL_FreeSurface(surface);
    return texture_name;
}
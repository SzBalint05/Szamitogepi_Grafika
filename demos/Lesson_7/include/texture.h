#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>

typedef GLubyte Pixel[3];

/**
 * Load texture from file and returns with the texture name.
 */
GLuint load_texture(char* filename);

GLuint generate_checkerboard_texture();

GLuint generate_circle_texture();

GLuint generate_gradient_texture();

#endif /* TEXTURE_H */
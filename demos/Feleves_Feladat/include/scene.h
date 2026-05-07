#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "texture.h"
#include <GL/gl.h>

#define MAP_SIZE 128
#define TILE_SIZE 2.0f

typedef struct{
    Material material;
    GLuint ground_texture;
    float heights[MAP_SIZE][MAP_SIZE];
}Scene;

void init_scene(Scene* scene);
void set_lighting();
void set_material(const Material* material);
void update_scene(Scene* scene, double time);
void render_scene(const Scene* scene);
float get_terrain_height(const Scene* scene, float x, float y);

#endif // SCENE_H
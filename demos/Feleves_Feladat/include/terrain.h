#ifndef TERRAIN_H
#define TERRAIN_H

#include "scene.h"
#include "utils.h"
#include <stdbool.h>

void init_terrain(Scene* scene);
void render_terrain(const Scene* scene);
void render_props(const Scene* scene);

void draw_props(const Prop* props, int count, GLuint texture, const Model* model);

float get_terrain_height(const Scene* scene, float x, float y);
float get_surface_height(const Scene* scene, float x, float y);
bool is_colliding(const Scene* scene, float x, float y, float z, float check_radius);

void destroy_terrain(Scene* scene);

#endif // TERRAIN_H
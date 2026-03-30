#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "texture.h"

typedef struct Scene
{
    Material material;
    GLuint base_texture;
    GLuint checkerboard_texture;
    GLuint circle_texture;
    GLuint gradient_texture;
    double time;
} Scene;

void init_scene(Scene* scene);
void set_lighting();
void set_material(const Material* material);
void update_scene(Scene* scene, double time);
void render_scene(const Scene* scene);
void draw_origin();
void draw_custom_cube(const Scene* scene);
void draw_ortho_image(GLuint texture_id);

#endif /* SCENE_H */
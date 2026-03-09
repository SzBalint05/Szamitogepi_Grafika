#ifndef SCENE_H
#define SCENE_H

#include "camera.h"

typedef struct Scene
{
    double sphere_rotation;
} Scene;

/**
 * Initialize the scene by loading models.
 */
void init_scene(Scene* scene);

/**
 * Update the scene.
 */
void update_scene(Scene* scene);

/**
 * Render the scene objects.
 */
void render_scene(const Scene* scene);

/**
 * Draw the origin of the world coordinate system.
 */
void draw_origin();


void draw_colorful_triangle();

void draw_chessboard();

void draw_tessellated_sphere(double rotation);

void draw_cylinder();

void draw_cone();

#endif /* SCENE_H */

#ifndef PHYSICS_H
#define PHYSICS_H

#include "camera.h"
#include "scene.h"

void apply_physics(Camera* camera, const Scene* scene, float old_x, float old_y, double elapsed_time);

#endif // PHYSICS_H
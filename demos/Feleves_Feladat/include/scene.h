#ifndef SCENE_H
#define SCENE_H

#include "camera.h"

typedef struct{
    int dummy; // Még nem csinál semmit
}Scene;

void init_scene(Scene* scene);
void update_scene(Scene* scene, double time);
void render_scene(const Scene* scene);

#endif // SCENE_H
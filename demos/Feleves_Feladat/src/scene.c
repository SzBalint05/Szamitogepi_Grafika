#include "scene.h"
#include <GL/gl.h>

void init_scene(Scene* scene){
    (void)scene;
}

void update_scene(Scene* scene, double time){
    (void)scene;
    (void)time;
}

void render_scene(const Scene* scene){
    (void)scene;

    glBegin(GL_LINES);
    glColor3f(0.5f, 0.5f, 0.5f);
    
    for (int i = -50; i <= 50; i += 2){
        glVertex3f((float)i, -50.0f, 0.0f);
        glVertex3f((float)i,  50.0f, 0.0f);
        glVertex3f(-50.0f, (float)i, 0.0f);
        glVertex3f( 50.0f, (float)i, 0.0f);
    }
    glEnd();
}
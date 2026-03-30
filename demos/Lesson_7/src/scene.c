#include "scene.h"
#include <math.h>

void init_scene(Scene* scene)
{
    scene->base_texture = load_texture("assets/textures/cube.png");
    scene->checkerboard_texture = generate_checkerboard_texture();
    scene->circle_texture = generate_circle_texture();
    scene->gradient_texture = generate_gradient_texture();
    scene->time = 0.0;

    scene->material.ambient.red = 0.2f; scene->material.ambient.green = 0.2f; scene->material.ambient.blue = 0.2f;
    scene->material.diffuse.red = 0.8f; scene->material.diffuse.green = 0.8f; scene->material.diffuse.blue = 0.8f;
    scene->material.specular.red = 0.0f; scene->material.specular.green = 0.0f; scene->material.specular.blue = 0.0f;
    scene->material.shininess = 10.0f;
}

void set_lighting()
{
    float ambient_light[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float specular_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float position[] = { 5.0f, 5.0f, 10.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material)
{
    float ambient[] = { material->ambient.red, material->ambient.green, material->ambient.blue };
    float diffuse[] = { material->diffuse.red, material->diffuse.green, material->diffuse.blue };
    float specular[] = { material->specular.red, material->specular.green, material->specular.blue };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &(material->shininess));
}

void update_scene(Scene* scene, double time)
{
    scene->time += time;
}

void draw_custom_cube(const Scene* scene)
{

    if (fmod(scene->time, 2.0) < 1.0) {
        glBindTexture(GL_TEXTURE_2D, scene->base_texture);
    } else {
        glBindTexture(GL_TEXTURE_2D, scene->checkerboard_texture);
    }
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, scene->checkerboard_texture);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, scene->checkerboard_texture);
    float offset = fmod(scene->time * 0.5, 1.0);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f + offset, 0.0f + offset); glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f + offset, 1.0f + offset); glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f + offset, 1.0f + offset); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f + offset, 0.0f + offset); glVertex3f( 1.0f,  1.0f, -1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, scene->base_texture);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, scene->gradient_texture);
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, scene->circle_texture);
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}

void render_scene(const Scene* scene)
{
    set_material(&(scene->material));
    set_lighting();
    draw_origin();

    draw_custom_cube(scene);

    draw_ortho_image(scene->circle_texture);
}

void draw_ortho_image(GLuint texture_id)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1); 

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(10.0f, 10.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(160.0f, 10.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(160.0f, 160.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(10.0f, 160.0f);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void draw_origin()
{
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(1, 0, 0);
    glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);
    glEnd();
}
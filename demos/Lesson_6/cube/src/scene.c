#include "scene.h"

#include <obj/load.h>
#include <obj/draw.h>

void init_scene(Scene* scene)
{
    // load_model(&(scene->cube), "assets/models/cube.obj");
    // scene->texture_id = load_texture("assets/textures/cube.png");
    load_model(&(scene->cube), "assets/models/cube.obj");
    load_model(&(scene->cat), "assets/models/cat.obj");
    scene->texture_id = load_texture("assets/textures/cube.png");

    glBindTexture(GL_TEXTURE_2D, scene->texture_id);

    scene->material.ambient.red = 1.0;
    scene->material.ambient.green = 1.0;
    scene->material.ambient.blue = 1.0;

    scene->material.diffuse.red = 1.0;
    scene->material.diffuse.green = 1.0;
    scene->material.diffuse.blue = 1.0;

    scene->material.specular.red = 0.0;
    scene->material.specular.green = 0.0;
    scene->material.specular.blue = 0.0;

    scene->material.shininess = 1.0;

    scene->cat_rotation = 0.0;
}

void set_lighting()
{
    float ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float diffuse_light[] = { 1.0f, 1.0f, 1.0, 1.0f };
    float specular_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float position[] = { 0.0f, 0.0f, 10.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material)
{
    float ambient_material_color[] = {
        material->ambient.red,
        material->ambient.green,
        material->ambient.blue
    };

    float diffuse_material_color[] = {
        material->diffuse.red,
        material->diffuse.green,
        material->diffuse.blue
    };

    float specular_material_color[] = {
        material->specular.red,
        material->specular.green,
        material->specular.blue
    };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material_color);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &(material->shininess));
}

void update_scene(Scene* scene, double time)
{
    scene->cat_rotation += 50.0 * time;
    if (scene->cat_rotation > 360.0) {
        scene->cat_rotation -= 360.0;
    }
}

void render_scene(const Scene* scene)
{
    set_material(&(scene->material));
    set_lighting();
    draw_origin();

    glBindTexture(GL_TEXTURE_2D, scene->texture_id);
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            glPushMatrix();

            glTranslatef(x * 2.0f, y * 2.0f, 0.0f);
            draw_model(&(scene->cube));
            draw_model(&(scene->cube));
            glPopMatrix();
        }
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(-5.0f, 3.0f, 0.0f); 
    glScalef(5.0f, 5.0f, 5.0f); 
    glRotatef((float)scene->cat_rotation, 0.0f, 0.0f, 1.0f);
    glColor3f(1.0f, 0.0f, 1.0f); 
    draw_model(&(scene->cat));
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

void draw_origin()
{
    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);

    glEnd();
}

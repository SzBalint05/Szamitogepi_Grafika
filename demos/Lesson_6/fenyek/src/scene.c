#include "scene.h"
#include <math.h>
#include <obj/load.h>
#include <obj/draw.h>

void init_scene(Scene* scene)
{
    load_model(&(scene->cube), "assets/models/cube.obj");
    scene->texture_id = load_texture("assets/textures/cube.png");

    load_model(&(scene->extra_model), "assets/models/duck.obj");
    scene->extra_texture_id = load_texture("assets/textures/duck.jpg");

    scene->rotation_angle = 0.0f;
    scene->light_intensity = 1.0f;
    scene->light_time = 0.0f;

    scene->material.ambient.red = 0.2;
    scene->material.ambient.green = 0.2;
    scene->material.ambient.blue = 0.2;

    scene->material.diffuse.red = 0.6;
    scene->material.diffuse.green = 0.6;
    scene->material.diffuse.blue = 0.6;

    scene->material.specular.red = 1.0;
    scene->material.specular.green = 1.0;
    scene->material.specular.blue = 1.0;

    scene->material.shininess = 100.0;
}

void set_lighting(const Scene* scene)
{
    float r = (sin(scene->light_time * 2.0) + 1.0) / 2.0;
    float g = (cos(scene->light_time * 1.5) + 1.0) / 2.0;
    float b = 1.0f;
    float intensity = scene->light_intensity;

    float ambient_light[] = { 0.1f * intensity, 0.1f * intensity, 0.1f * intensity, 1.0f };
    float diffuse_light[] = { r * intensity, g * intensity, b * intensity, 1.0f };
    float specular_light[] = { 1.0f * intensity, 1.0f * intensity, 1.0f * intensity, 1.0f };
    
    float light_x = 5.0f * cos(scene->light_time);
    float light_z = 5.0f * sin(scene->light_time);
    float position[] = { light_x, 5.0f, light_z, 1.0f };

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
    scene->light_time += time;

    scene->rotation_angle += 45.0 * time;
    if (scene->rotation_angle > 360.0) {
        scene->rotation_angle -= 360.0;
    }
}

void render_scene(const Scene* scene)
{
    set_material(&(scene->material));
    set_lighting(scene);
    draw_origin();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, scene->texture_id);
    glRotatef(scene->rotation_angle, 0.0f, 1.0f, 0.0f);
    draw_model(&(scene->cube));
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, scene->extra_texture_id);
    glTranslatef(2.5f, 0.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    glRotatef(-scene->rotation_angle * 1.5f, 1.0f, 0.0f, 0.0f);
    draw_model(&(scene->extra_model));
    glPopMatrix();
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
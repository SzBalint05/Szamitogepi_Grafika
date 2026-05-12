#include "scene.h"
#include "terrain.h"
#include "ghost.h"
#include <obj/load.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void set_lighting(){
    float ambient_light[] = {0.4f, 0.4f, 0.4f, 1.0f};
    float diffuse_light[] = {0.4f, 0.4f, 0.4f, 1.0f};
    float specular_light[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float position[] = {0.0f, 0.0f, 1.0f, 0.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material){
    float ambient[] = {material->ambient.red, material->ambient.green, material->ambient.blue};
    float diffuse[] = {material->diffuse.red, material->diffuse.green, material->diffuse.blue};
    float specular[] = {material->specular.red, material->specular.green, material->specular.blue};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &(material->shininess));
}

void init_scene(Scene* scene){
    srand(time(NULL));

    // Kezdeti értékek
    scene->flashlight_level = 0;
    scene->battery = 100.0f;
    scene->hp = 100;
    scene->kills = 0;
    scene->is_shooting = false;
    scene->shot_timer = 0.0f;
    scene->gun_cooldown = 0.0f;
    scene->damage_indicator_timer = 0.0f;

    // HUD textúrák
    scene->hud_flashlight_texture = load_texture("assets/textures/flashlight.png");
    glBindTexture(GL_TEXTURE_2D, scene->hud_flashlight_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    scene->gun_texture = load_texture("assets/textures/gun.png");
    glBindTexture(GL_TEXTURE_2D, scene->gun_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    init_terrain(scene);

    // Szellem
    load_model(&(scene->ghost_model), "assets/models/ghost.obj");

    // Szellemek elhelyezése
    for(int i = 0; i < MAX_GHOSTS; i++){
        scene->ghosts[i].is_alive = false;
    }
    for(int i = 0; i < 3; i++){
        spawn_ghost(scene, 64.0f, 64.0f);
    }

    // Anyagbeállítások
    scene->material.ambient.red = 0.3f; scene->material.ambient.green = 0.3f; scene->material.ambient.blue = 0.3f;
    scene->material.diffuse.red = 0.3f; scene->material.diffuse.green = 0.3f; scene->material.diffuse.blue = 0.3f;
    scene->material.specular.red = 0.5f; scene->material.specular.green = 0.5f; scene->material.specular.blue = 0.5f;
    scene->material.shininess = 0.0f;
}

void update_scene(Scene* scene, double time, const Camera* camera){
    // Zseblámpa aku csökkentése
    if(scene->flashlight_level > 0){
        float drain_rates[] = {0.0f, 1.0f, 2.0f, 3.0f};
        scene->battery -= drain_rates[scene->flashlight_level] * time;

        if(scene->battery <= 0.0f){
            scene->battery = 0.0f;
            scene->flashlight_level = 0;
        }
    }

    // Fegyver időzítő frissítése
    if(scene->is_shooting){
        scene->shot_timer -= time;
        if(scene->shot_timer <= 0.0f){
            scene->is_shooting = false;
        }
    }
    if(scene->gun_cooldown > 0.0f) scene->gun_cooldown -= time;
    if(scene->damage_indicator_timer > 0.0f) scene->damage_indicator_timer -= time;

    update_ghosts(scene, time, camera);
}

void render_scene(const Scene* scene){
    set_material(&(scene->material));
    set_lighting();

    render_terrain(scene);
    render_props(scene);
    render_ghosts(scene);

    set_material(&(scene->material));

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
}

void shoot_weapon(Scene* scene, const Camera* camera){
    if(scene->is_shooting || scene->gun_cooldown > 0.0f) return;

    scene->is_shooting = true;
    scene->shot_timer = 0.05f;
    scene->gun_cooldown = 0.5f;

    float dir_x = cos(degree_to_radian(camera->rotation.z)) * cos(degree_to_radian(camera->rotation.x));
    float dir_y = sin(degree_to_radian(camera->rotation.z)) * cos(degree_to_radian(camera->rotation.x));
    float dir_z = sin(degree_to_radian(camera->rotation.x));

    check_ghost_hits(scene, camera, dir_x, dir_y, dir_z);
}

void destroy_scene(Scene* scene) {
    destroy_terrain(scene);

    free_model(&(scene->ghost_model));
    glDeleteTextures(1, &(scene->hud_flashlight_texture));
    glDeleteTextures(1, &(scene->gun_texture));
}
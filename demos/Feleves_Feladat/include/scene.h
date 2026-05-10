#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "texture.h"
#include <obj/model.h>
#include <GL/gl.h>
#include <stdbool.h>

#define MAP_SIZE 128
#define TILE_SIZE 2.0f
#define NUM_TREES 100
#define NUM_ROCKS 70
#define SPAWN_RADIUS 5.0f
#define PLAYER_RADIUS 1.0f
#define MAX_GHOSTS 6

typedef struct Prop{
    float x;
    float y;
    float z;
    float rotation;
}Prop;

typedef struct Ghost{
    float x;
    float y;
    float z;
    bool is_alive;
    bool is_illuminated;
    bool is_dying;
    int hp;
    float scale;
    float alpha;
    bool killed_by_player;
    float damage_timer;
}Ghost;

typedef struct{
    Material material;
    GLuint ground_texture;
    float heights[MAP_SIZE][MAP_SIZE];

    Model tree_model;
    GLuint tree_texture;
    Prop trees[NUM_TREES];

    Model rock_model;
    GLuint rock_texture;
    Prop rocks[NUM_ROCKS];

    int flashlight_level;
    float battery;
    GLuint hud_flashlight_texture;

    int hp;
    int kills;
    float damage_indicator_timer;

    Model ghost_model;
    Ghost ghosts[MAX_GHOSTS];

    GLuint gun_texture;
    bool is_shooting;
    float shot_timer;
    float gun_cooldown;
}Scene;

void init_scene(Scene* scene);
void set_lighting();
void set_material(const Material* material);
void update_scene(Scene* scene, double time, const Camera* camera);
void render_scene(const Scene* scene);
float get_terrain_height(const Scene* scene, float x, float y);
float get_surface_height(const Scene* scene, float x, float y);
bool is_colliding(const Scene* scene, float x, float y, float z, float check_radius);
void spawn_ghost(Scene* scene, float player_x, float player_y);
void shoot_weapon(Scene* scene, const Camera* camera);

#endif // SCENE_H
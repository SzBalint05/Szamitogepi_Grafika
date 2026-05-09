#include "scene.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <obj/load.h>
#include <obj/draw.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void set_lighting(){
    float ambient_light[] = {0.3f, 0.3f, 0.3f, 1.0f};
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

void spawn_ghost(Scene* scene, float player_x, float player_y){
    int slot = -1;
    for(int i = 0; i < MAX_GHOSTS; i++){
        if(!scene->ghosts[i].is_alive){
            slot = i;
            break;
        }
    }
    if(slot == -1) return;

    bool ok = false;
    while(!ok){
        float test_x = 1.0f + ((float)rand() / RAND_MAX) * (MAP_SIZE - 3.0f);
        float test_y = 1.0f + ((float)rand() / RAND_MAX) * (MAP_SIZE - 3.0f);
        
        float dx = test_x - player_x;
        float dy = test_y - player_y;
        
        if(dx*dx + dy*dy > 100.0f){
            scene->ghosts[slot].x = test_x;
            scene->ghosts[slot].y = test_y;
            scene->ghosts[slot].z = get_terrain_height(scene, test_x, test_y) + 1.5f;
            scene->ghosts[slot].is_alive = true;
            scene->ghosts[slot].is_illuminated = false;
            scene->ghosts[slot].is_dying = false;
            scene->ghosts[slot].hp = 3;
            scene->ghosts[slot].scale = 0.02f;
            scene->ghosts[slot].alpha = 0.6f;
            ok = true;
        }
    }
}

void init_scene(Scene* scene){
    scene->ground_texture = load_texture("assets/textures/grass.png");
    SDL_Surface* raw_hm = IMG_Load("assets/textures/heightmap.png");
    SDL_Surface* hm = NULL;

    if(raw_hm != NULL){
        hm = SDL_ConvertSurfaceFormat(raw_hm, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(raw_hm);
    }else{
        printf("Can't find: assets/textures/heightmap.png\n");
    }
    
    for(int y = 0; y < MAP_SIZE; y++){
        for(int x = 0; x < MAP_SIZE; x++){
            float height_val = 0.0f;

            if(hm != NULL && x < hm->w && y < hm->h){
                Uint32 pixel = ((Uint32*)hm->pixels)[y * hm->w + x];
                Uint8 r, g, b, a;
                SDL_GetRGBA(pixel, hm->format, &r, &g, &b, &a);
                height_val = (float)r / 25.0f;
            }
            scene->heights[y][x] = height_val;
        }
    }

    if(hm) SDL_FreeSurface(hm);

    srand(time(NULL));

    // Tereptárgyak és szellemek
    load_model(&(scene->tree_model), "assets/models/Tree Type1 04.obj");
    scene->tree_texture = load_texture("assets/textures/Colorsheet Tree Normal.png");

    load_model(&(scene->rock_model), "assets/models/Rock Type2 03.obj");
    scene->rock_texture = load_texture("assets/textures/Colorsheet Rock Grey.png");

    load_model(&(scene->ghost_model), "assets/models/ghost.obj");
    scene->gun_cooldown = 0.0f;

    // Fák elhelyezése
    for(int i = 0; i < NUM_TREES; i++){
        bool ok = false;

        while(!ok){
            float test_x = 1.0f + ((float)rand() / RAND_MAX) * (MAP_SIZE - 3.0f);
            float test_y = 1.0f + ((float)rand() / RAND_MAX) * (MAP_SIZE - 3.0f);
            ok = true;

            for(int j = 0; j < i; j++){
                float dx = test_x - scene->trees[j].x;
                float dy = test_y - scene->trees[j].y;
                if(dx*dx + dy*dy < (SPAWN_RADIUS * SPAWN_RADIUS)){
                    ok = false;
                    break;
                }
            }
            if(ok){
                scene->trees[i].x = test_x;
                scene->trees[i].y = test_y;
            }
        }
        scene->trees[i].z = get_terrain_height(scene, scene->trees[i].x, scene->trees[i].y);
        scene->trees[i].rotation = ((float)rand() / RAND_MAX) * 360.0f;
    }

    // Kövek elhelyezése
    for(int i = 0; i < NUM_ROCKS; i++){
        bool ok = false;

        while(!ok){
            float test_x = 1.0f + ((float)rand() / RAND_MAX) * (MAP_SIZE - 3.0f);
            float test_y = 1.0f + ((float)rand() / RAND_MAX) * (MAP_SIZE - 3.0f);
            
            if(!is_colliding(scene, test_x, test_y, 0.0f, SPAWN_RADIUS)){
                ok = true;
                scene->rocks[i].x = test_x;
                scene->rocks[i].y = test_y;
            }
        }
        scene->rocks[i].z = get_terrain_height(scene, scene->rocks[i].x, scene->rocks[i].y);
        scene->rocks[i].rotation = ((float)rand() / RAND_MAX) * 360.0f;
    }

    // Szellemek elhelyezése
    for(int i = 0; i < MAX_GHOSTS; i++){
        scene->ghosts[i].is_alive = false;
    }
    for(int i = 0; i < 3; i++){
        spawn_ghost(scene, 64.0f, 64.0f);
    }

    scene->material.ambient.red = 0.3f; scene->material.ambient.green = 0.3f; scene->material.ambient.blue = 0.3f;
    scene->material.diffuse.red = 0.3f; scene->material.diffuse.green = 0.3f; scene->material.diffuse.blue = 0.3f;
    scene->material.specular.red = 0.5f; scene->material.specular.green = 0.5f; scene->material.specular.blue = 0.5f;
    scene->material.shininess = 0.0f;

    // Kezdeti értékek
    scene->flashlight_level = 0;
    scene->battery = 100.0f;
    scene->hp = 100;
    scene->kills = 0;
    scene->is_shooting = false;
    scene->shot_timer = 0.0f;

    // HUD textúrák
    scene->hud_flashlight_texture = load_texture("assets/textures/flashlight.png");
    glBindTexture(GL_TEXTURE_2D, scene->hud_flashlight_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    scene->gun_texture = load_texture("assets/textures/gun.png");
    glBindTexture(GL_TEXTURE_2D, scene->gun_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void update_scene(Scene* scene, double time, const Camera* camera){
    if(scene->flashlight_level > 0){
        float drain_rates[] = {0.0f, 1.0f, 2.0f, 3.0f};
        scene->battery -= drain_rates[scene->flashlight_level] * time;

        if(scene->battery <= 0.0f){
            scene->battery = 0.0f;
            scene->flashlight_level = 0;
        }
    }

    if(scene->is_shooting){
        scene->shot_timer -= time;
        if(scene->shot_timer <= 0.0f){
            scene->is_shooting = false;
        }
    }

    if(scene->gun_cooldown > 0.0f) scene->gun_cooldown -= time;
    if(scene->damage_indicator_timer > 0.0f) scene->damage_indicator_timer -= time;
    float cam_dx = cos(degree_to_radian(camera->rotation.z)) * cos(degree_to_radian(camera->rotation.x));
    float cam_dy = sin(degree_to_radian(camera->rotation.z)) * cos(degree_to_radian(camera->rotation.x));
    float cam_dz = sin(degree_to_radian(camera->rotation.x));

    for(int i = 0; i < MAX_GHOSTS; i++){
        if(!scene->ghosts[i].is_alive) continue;

        if(scene->ghosts[i].is_dying){
            scene->ghosts[i].scale -= 0.02f * time;
            scene->ghosts[i].alpha -= 0.7f * time;
            
            if(scene->ghosts[i].scale <= 0.0f || scene->ghosts[i].alpha <= 0.0f){
                scene->ghosts[i].is_alive = false;
                scene->kills++;
                spawn_ghost(scene, camera->position.x, camera->position.y);
                spawn_ghost(scene, camera->position.x, camera->position.y);
            }
            continue;
        }

        float dir_x = camera->position.x - scene->ghosts[i].x;
        float dir_y = camera->position.y - scene->ghosts[i].y;
        float dist2d = sqrt(dir_x * dir_x + dir_y * dir_y);

        if(dist2d > 0.1f){
            float speed = 3.5f;

            scene->ghosts[i].x += (dir_x / dist2d) * speed * time;
            scene->ghosts[i].y += (dir_y / dist2d) * speed * time;
            scene->ghosts[i].z = get_terrain_height(scene, scene->ghosts[i].x, scene->ghosts[i].y) + 1.5f;
        }

        if(!scene->ghosts[i].is_dying){
            float dx = fabs(scene->ghosts[i].x - camera->position.x);
            float dy = fabs(scene->ghosts[i].y - camera->position.y);
            float dz = fabs(scene->ghosts[i].z - camera->position.z); 

            if(dx < 1.5f && dy < 1.5f && dz < 3.0f){
                scene->hp -= 25;
                scene->damage_indicator_timer = 0.5f;
                scene->ghosts[i].hp = 0;
                scene->ghosts[i].is_dying = true;
                scene->ghosts[i].alpha = 1.0f; 
                spawn_ghost(scene, camera->position.x, camera->position.y);
            }
        }

        scene->ghosts[i].is_illuminated = false;
        
        if(scene->flashlight_level > 0){
            float dx = scene->ghosts[i].x - camera->position.x;
            float dy = scene->ghosts[i].y - camera->position.y;
            float dz = scene->ghosts[i].z - camera->position.z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);

            float max_dist = 0.0f;
            float min_dot = 1.0f;

            if(scene->flashlight_level == 1){
                max_dist = 12.0f;
                min_dot = cos(degree_to_radian(25.0f));
            }else if(scene->flashlight_level == 2){
                max_dist = 20.0f; 
                min_dot = cos(degree_to_radian(22.0f));
            }else if(scene->flashlight_level == 3){
                max_dist = 30.0f;
                min_dot = cos(degree_to_radian(18.0f));
            }

            if(dist < max_dist){ 
                float dot = (dx/dist)*cam_dx + (dy/dist)*cam_dy + (dz/dist)*cam_dz;
                if(dot > min_dot){ 
                    scene->ghosts[i].is_illuminated = true;
                }
            }
        }
    }
}

void render_scene(const Scene* scene){
    set_material(&(scene->material));
    set_lighting();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, scene->ground_texture);
    glBegin(GL_QUADS);

    for(int y = 0; y < MAP_SIZE - 1; y++){
        for(int x = 0; x < MAP_SIZE - 1; x++){
            glNormal3f(0.0f, 0.0f, 1.0f);
            
            float u = (float)x / 4.0f; 
            float v = (float)y / 4.0f;

            glTexCoord2f(u, v);
            glVertex3f(x, y, scene->heights[y][x]);

            glTexCoord2f(u + 0.25f, v);
            glVertex3f(x + 1, y, scene->heights[y][x + 1]);

            glTexCoord2f(u + 0.25f, v + 0.25f);
            glVertex3f(x + 1, y + 1, scene->heights[y + 1][x + 1]);

            glTexCoord2f(u, v + 0.25f);
            glVertex3f(x, y + 1, scene->heights[y + 1][x]);
        }
    }
    glEnd();

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.2f);

    glBindTexture(GL_TEXTURE_2D, scene->tree_texture);
    for(int i = 0; i < NUM_TREES; i++){
        glPushMatrix();
        glTranslatef(scene->trees[i].x, scene->trees[i].y, scene->trees[i].z);
        glRotatef(scene->trees[i].rotation, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(2.5f, 2.5f, 2.5f);
        
        draw_model(&(scene->tree_model));
        
        glPopMatrix();
    }

    glBindTexture(GL_TEXTURE_2D, scene->rock_texture);
    for(int i = 0; i < NUM_ROCKS; i++){
        glPushMatrix();
        glTranslatef(scene->rocks[i].x, scene->rocks[i].y, scene->rocks[i].z);
        glRotatef(scene->rocks[i].rotation, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(2.5f, 2.5f, 2.5f);
        draw_model(&(scene->rock_model));
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
    float zero_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero_specular);

    for(int i = 0; i < MAX_GHOSTS; i++){
        if(!scene->ghosts[i].is_alive) continue;

        if(scene->ghosts[i].is_dying){
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            float ghost_dying[] = {0.2f, 0.1f, 0.1f, scene->ghosts[i].alpha}; 
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ghost_dying);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ghost_dying);
            
        }else if(scene->ghosts[i].is_illuminated){
            glDisable(GL_BLEND);
            float ghost_light[] = {0.1f, 0.1f, 0.1f, 1.0f}; 
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ghost_light);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ghost_light);
            
        }else{
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            float ghost_dark[] = {0.0f, 0.0f, 0.0f, scene->ghosts[i].alpha};
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ghost_dark);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ghost_dark);
        }

        glPushMatrix();
        glTranslatef(scene->ghosts[i].x, scene->ghosts[i].y, scene->ghosts[i].z);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(scene->ghosts[i].scale, scene->ghosts[i].scale, scene->ghosts[i].scale);
        draw_model(&(scene->ghost_model));
        glPopMatrix();
    }
    glDisable(GL_BLEND);
    set_material(&(scene->material));

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
}

float get_terrain_height(const Scene* scene, float x, float y){
    if(x < 0.0f || x >= MAP_SIZE - 1 || y < 0.0f || y >= MAP_SIZE - 1){
        return 0.0f;
    }

    int ix = (int)x;
    int iy = (int)y;

    float frac_x = x - ix;
    float frac_y = y - iy;

    float h1 = scene->heights[iy][ix];
    float h2 = scene->heights[iy][ix + 1];
    float h3 = scene->heights[iy + 1][ix];
    float h4 = scene->heights[iy + 1][ix + 1];

    float top_h = h1 * (1.0f - frac_x) + h2 * frac_x;
    float bot_h = h3 * (1.0f - frac_x) + h4 * frac_x;
    float final_height = top_h * (1.0f - frac_y) + bot_h * frac_y;

    return final_height;
}

bool is_colliding(const Scene* scene, float x, float y, float z, float half_size){
    for(int i = 0; i < NUM_TREES; i++){
        float dx = fabs(scene->trees[i].x - x);
        float dy = fabs(scene->trees[i].y - y);

        if(dx < half_size && dy < half_size && z < scene->trees[i].z + 9.9f) return true;
    }

    for(int i = 0; i < NUM_ROCKS; i++){
        float dx = fabs(scene->rocks[i].x - x);
        float dy = fabs(scene->rocks[i].y - y);
        
        if(dx < half_size && dy < half_size && z < scene->rocks[i].z + 0.9f) return true;
    }
    return false;
}

float get_surface_height(const Scene* scene, float x, float y){
    float max_z = get_terrain_height(scene, x, y);

    for(int i = 0; i < NUM_ROCKS; i++){
        float dx = fabs(scene->rocks[i].x - x);
        float dy = fabs(scene->rocks[i].y - y);

        if(dx < PLAYER_RADIUS && dy < PLAYER_RADIUS){
            float rock_top = scene->rocks[i].z + 1.0f;
            if(rock_top > max_z) max_z = rock_top;
        }
    }
    return max_z;
}
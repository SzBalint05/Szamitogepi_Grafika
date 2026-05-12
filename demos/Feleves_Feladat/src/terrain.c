#include "terrain.h"
#include <obj/load.h>
#include <obj/draw.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void draw_props(const Prop* props, int count, GLuint texture, const Model* model){
    glBindTexture(GL_TEXTURE_2D, texture);

    for(int i = 0; i < count; i++){
        glPushMatrix();
        glTranslatef(props[i].x, props[i].y, props[i].z);
        glRotatef(props[i].rotation, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(2.5f, 2.5f, 2.5f);
        draw_model(model);
        glPopMatrix();
    }
}

void init_terrain(Scene* scene){
    // Textúrák és modellek betöltése
    load_model(&(scene->tree_model), "assets/models/Tree Type1 04.obj");
    scene->tree_texture = load_texture("assets/textures/Colorsheet Tree Normal.png");
    load_model(&(scene->rock_model), "assets/models/Rock Type2 03.obj");
    scene->rock_texture = load_texture("assets/textures/Colorsheet Rock Grey.png");
    scene->ground_texture = load_texture("assets/textures/grass.png");

    // Magasságtérkép beolvasása
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

    // Fák elhelyezése
    for(int i = 0; i < NUM_TREES; i++){
        bool ok = false;
        while(!ok){
            float test_x = random_float(1.0f, MAP_SIZE - 2.0f);
            float test_y = random_float(1.0f, MAP_SIZE - 2.0f);
            ok = true;
            float px = test_x - 64.0f;
            float py = test_y - 64.0f;

            if(px*px + py*py < 25.0f){
                ok = false;
                continue;
            }

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
            float test_x = random_float(1.0f, MAP_SIZE - 2.0f);
            float test_y = random_float(1.0f, MAP_SIZE - 2.0f);
            float px = test_x - 64.0f;
            float py = test_y - 64.0f;
            if(px*px + py*py < 25.0f) continue;

            if(!is_colliding(scene, test_x, test_y, 0.0f, SPAWN_RADIUS)){
                ok = true;
                scene->rocks[i].x = test_x;
                scene->rocks[i].y = test_y;
            }
        }
        scene->rocks[i].z = get_terrain_height(scene, scene->rocks[i].x, scene->rocks[i].y);
        scene->rocks[i].rotation = ((float)rand() / RAND_MAX) * 360.0f;
    }
}

void render_terrain(const Scene* scene){
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
}

void render_props(const Scene* scene){
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.2f);

    draw_props(scene->trees, NUM_TREES, scene->tree_texture, &(scene->tree_model));
    draw_props(scene->rocks, NUM_ROCKS, scene->rock_texture, &(scene->rock_model));
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
    return top_h * (1.0f - frac_y) + bot_h * frac_y;
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

void destroy_terrain(Scene* scene){
    glDeleteTextures(1, &(scene->tree_texture));
    glDeleteTextures(1, &(scene->rock_texture));
    glDeleteTextures(1, &(scene->ground_texture));

    free_model(&(scene->tree_model));
    free_model(&(scene->rock_model));
}
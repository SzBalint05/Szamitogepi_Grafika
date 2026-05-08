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
    float ambient_light[] = {0.6f, 0.6f, 0.6f, 1.0f};
    float diffuse_light[] = {0.4f, 0.4f, 0.4f, 1.0f};
    float specular_light[] = {0.3f, 0.3f, 0.3f, 1.0f};
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
    scene->ground_texture = load_texture("assets/textures/grass.png");
    SDL_Surface* raw_hm = IMG_Load("assets/textures/heightmap.png");
    SDL_Surface* hm = NULL;
    scene->hud_flashlight_texture = load_texture("assets/textures/flashlight.png");
    
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

    // Tereptárgyak
    load_model(&(scene->tree_model), "assets/models/Tree Type1 04.obj");
    scene->tree_texture = load_texture("assets/textures/Colorsheet Tree Normal.png");

    load_model(&(scene->rock_model), "assets/models/Rock Type2 03.obj");
    scene->rock_texture = load_texture("assets/textures/Colorsheet Rock Grey.png");

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

    for(int i = 0; i < NUM_ROCKS; i++){
        bool ok = false;

        while(!ok){
            float test_x = 1.0f + ((float)rand() / RAND_MAX) * (MAP_SIZE - 3.0f);
            float test_y = 1.0f + ((float)rand() / RAND_MAX) * (MAP_SIZE - 3.0f);
            
            if(!is_colliding(scene, test_x, test_y, SPAWN_RADIUS)){
                ok = true;
                scene->rocks[i].x = test_x;
                scene->rocks[i].y = test_y;
            }
        }
        scene->rocks[i].z = get_terrain_height(scene, scene->rocks[i].x, scene->rocks[i].y);
        scene->rocks[i].rotation = ((float)rand() / RAND_MAX) * 360.0f;
    }

    scene->material.ambient.red = 0.3f; scene->material.ambient.green = 0.3f; scene->material.ambient.blue = 0.3f;
    scene->material.diffuse.red = 0.3f; scene->material.diffuse.green = 0.3f; scene->material.diffuse.blue = 0.3f;
    scene->material.specular.red = 0.4f; scene->material.specular.green = 0.4f; scene->material.specular.blue = 0.4f;
    scene->material.shininess = 0.0f;

    scene->flashlight_level = 0;
    scene->battery = 100.0f;
}

void update_scene(Scene* scene, double time){
    if (scene->flashlight_level > 0) {
        float drain_rates[] = { 0.0f, 1.0f, 2.0f, 3.0f };
        scene->battery -= drain_rates[scene->flashlight_level] * time;


        if (scene->battery <= 0.0f) {
            scene->battery = 0.0f;
            scene->flashlight_level = 0;
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

bool is_colliding(const Scene* scene, float x, float y, float half_size){
    for(int i = 0; i < NUM_TREES; i++){
        float dx = fabs(scene->trees[i].x - x);
        float dy = fabs(scene->trees[i].y - y);
        if(dx < half_size && dy < half_size) return true;
    }

    for(int i = 0; i < NUM_ROCKS; i++){
        float dx = fabs(scene->rocks[i].x - x);
        float dy = fabs(scene->rocks[i].y - y);
        if(dx < half_size && dy < half_size) return true;
    }

    return false; 
}
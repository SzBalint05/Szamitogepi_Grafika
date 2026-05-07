#include "scene.h"
#include "utils.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>

void set_lighting(){
    float ambient_light[] = {0.6f, 0.6f, 0.6f, 1.0f};
    float diffuse_light[] = {0.4f, 0.4f, 0.4f, 1.0f};
    float specular_light[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float position[] = {0.0f, 0.0f, 1.0f, 0.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material){
    float ambient[] = { material->ambient.red, material->ambient.green, material->ambient.blue };
    float diffuse[] = { material->diffuse.red, material->diffuse.green, material->diffuse.blue };
    float specular[] = { material->specular.red, material->specular.green, material->specular.blue };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &(material->shininess));
}

void init_scene(Scene* scene){
    scene->ground_texture = load_texture("assets/grass.png");
    SDL_Surface* raw_hm = IMG_Load("assets/heightmap.png");
    SDL_Surface* hm = NULL;
    
    if(raw_hm != NULL){
        hm = SDL_ConvertSurfaceFormat(raw_hm, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(raw_hm);
    }else{
        printf("Can't find: assets/heightmap.png\n");
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

    scene->material.ambient.red = 0.3f; scene->material.ambient.green = 0.3f; scene->material.ambient.blue = 0.3f;
    scene->material.diffuse.red = 0.3f; scene->material.diffuse.green = 0.3f; scene->material.diffuse.blue = 0.3f;
    scene->material.specular.red = 0.0f; scene->material.specular.green = 0.0f; scene->material.specular.blue = 0.0f;
    scene->material.shininess = 0.0f;
}

void update_scene(Scene* scene, double time){
    (void)scene;
    (void)time;
}

void render_scene(const Scene* scene){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, scene->ground_texture);

    set_material(&(scene->material));

    set_lighting();

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
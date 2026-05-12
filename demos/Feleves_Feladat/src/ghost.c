#include "ghost.h"
#include "scene.h"
#include "terrain.h"
#include "utils.h"
#include <math.h>
#include <GL/gl.h>
#include <obj/draw.h>

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
        float test_x = random_float(1.0f, MAP_SIZE - 2.0f);
        float test_y = random_float(1.0f, MAP_SIZE - 2.0f);
        
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
            scene->ghosts[slot].damage_timer = 0.0f;
            ok = true;
        }
    }
}

void update_ghosts(Scene* scene, double time, const Camera* camera){
    float cam_dx = cos(degree_to_radian(camera->rotation.z)) * cos(degree_to_radian(camera->rotation.x));
    float cam_dy = sin(degree_to_radian(camera->rotation.z)) * cos(degree_to_radian(camera->rotation.x));
    float cam_dz = sin(degree_to_radian(camera->rotation.x));

    for(int i = 0; i < MAX_GHOSTS; i++){
        if(!scene->ghosts[i].is_alive) continue;

        if(scene->ghosts[i].damage_timer > 0.0f){
            scene->ghosts[i].damage_timer -= time;
        }

        if(scene->ghosts[i].is_dying){
            scene->ghosts[i].scale -= 0.02f * time;
            scene->ghosts[i].alpha -= 0.7f * time;
            
            if(scene->ghosts[i].scale <= 0.0f || scene->ghosts[i].alpha <= 0.0f){
                scene->ghosts[i].is_alive = false;
                if(scene->ghosts[i].hp == 0) scene->kills++;
                spawn_ghost(scene, camera->position.x, camera->position.y);
                spawn_ghost(scene, camera->position.x, camera->position.y);
            }
            continue;
        }

        float dir_x = camera->position.x - scene->ghosts[i].x;
        float dir_y = camera->position.y - scene->ghosts[i].y;
        float dist2d = sqrt(dir_x * dir_x + dir_y * dir_y);

        if(dist2d > 0.1f){
            float speed = 3.0f;
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
                scene->ghosts[i].hp = -99;
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

void render_ghosts(const Scene* scene){
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
        
        }else if(scene->ghosts[i].damage_timer > 0.0f){
            glDisable(GL_BLEND);
            float ghost_hit[] = {0.2f, 0.1f, 0.1f, 1.0f};
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ghost_hit);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ghost_hit);

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
}

void check_ghost_hits(Scene* scene, const Camera* camera, float dir_x, float dir_y, float dir_z){
    for(int i = 0; i < MAX_GHOSTS; i++){
        if(scene->ghosts[i].is_alive && scene->ghosts[i].is_illuminated && !scene->ghosts[i].is_dying){
            
            float vx = scene->ghosts[i].x - camera->position.x;
            float vy = scene->ghosts[i].y - camera->position.y;
            float vz = scene->ghosts[i].z - camera->position.z;
            float t = vx*dir_x + vy*dir_y + vz*dir_z;

            if(t > 0.0f && t < 50.0f){
                float hit_x = camera->position.x + dir_x * t;
                float hit_y = camera->position.y + dir_y * t;
                float hit_z = camera->position.z + dir_z * t;
                float dist_sq = pow(scene->ghosts[i].x - hit_x, 2) +  pow(scene->ghosts[i].y - hit_y, 2) + pow(scene->ghosts[i].z - hit_z, 2);

                if(dist_sq < 4.0f){
                    scene->ghosts[i].hp--;
                    scene->ghosts[i].damage_timer = 0.15f;

                    if(scene->ghosts[i].hp <= 0){
                        scene->ghosts[i].is_dying = true;
                        scene->ghosts[i].alpha = 1.0f;

                        scene->battery += 5.0f;
                        if (scene->battery >= 100.0f) scene->battery = 100.0f;
                    }
                    break;
                }
            }
        }
    }
}
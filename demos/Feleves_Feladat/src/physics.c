#include "physics.h"
#include "terrain.h"

void apply_physics(Camera* camera, const Scene* scene, float old_x, float old_y, double elapsed_time){
    float current_z = camera->position.z - 1.5f;
    float new_x = camera->position.x;
    float new_y = camera->position.y;

    camera->position.x = new_x;
    if(is_colliding(scene, camera->position.x, camera->position.y, current_z, PLAYER_RADIUS)){
        camera->position.x = old_x; 
    }

    camera->position.y = new_y;
    if(is_colliding(scene, camera->position.x, camera->position.y, current_z, PLAYER_RADIUS)){
        camera->position.y = old_y; 
    }

    float target_z = get_surface_height(scene, camera->position.x, camera->position.y);

    if(camera->is_jumping){
        current_z += camera->vertical_speed * elapsed_time;
        camera->vertical_speed -= 15.0f * elapsed_time;

        if(current_z <= target_z){
            current_z = target_z;
            camera->is_jumping = false;
            camera->vertical_speed = 0.0f;
        }
    }else{
        if(current_z > target_z + 0.3f){
            camera->is_jumping = true;
            camera->vertical_speed = 0.0f;
        }else{
            current_z = target_z;
        }
    }
    camera->position.z = current_z + 1.5f;
}
#include "camera.h"
#include <GL/gl.h>
#include <math.h>

void init_camera(Camera* camera){
    camera->position.x = 64.0;
    camera->position.y = 64.0;
    camera->position.z = 1.5;
    camera->rotation.x = 0.0;
    camera->rotation.y = 0.0;
    camera->rotation.z = 90.0;
    camera->speed.x = 0.0;
    camera->speed.y = 0.0;
    camera->speed.z = 0.0;
    camera->is_jumping = false;
    camera->vertical_speed = 0.0f;
}

void update_camera(Camera* camera, double time){
    double angle = degree_to_radian(camera->rotation.z);
    double side_angle = degree_to_radian(camera->rotation.z + 90.0);

    double dx = cos(angle) * camera->speed.y + cos(side_angle) * camera->speed.x;
    double dy = sin(angle) * camera->speed.y + sin(side_angle) * camera->speed.x;
    double current_speed = sqrt(dx*dx + dy*dy);

    if(current_speed > 5.0){
        dx = (dx / current_speed) * 5.0;
        dy = (dy / current_speed) * 5.0;
    }

    camera->position.x += dx * time;
    camera->position.y += dy * time;
}

void set_view(const Camera* camera){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-(camera->rotation.x + 90), 1.0, 0, 0);
    glRotatef(-(camera->rotation.z - 90), 0, 0, 1.0);
    glTranslatef(-camera->position.x, -camera->position.y, -camera->position.z);
}

void rotate_camera(Camera* camera, double horizontal, double vertical){
    camera->rotation.z += horizontal;
    camera->rotation.x += vertical;

    if(camera->rotation.z < 0) camera->rotation.z += 360.0;
    if(camera->rotation.z > 360.0) camera->rotation.z -= 360.0;
    
    if(camera->rotation.x < -89.0) camera->rotation.x = -89.0;
    if(camera->rotation.x > 89.0) camera->rotation.x = 89.0;
}

void set_camera_speed(Camera* camera, double speed){
    camera->speed.y = speed;
}

void set_camera_side_speed(Camera* camera, double speed){
    camera->speed.x = speed;
}
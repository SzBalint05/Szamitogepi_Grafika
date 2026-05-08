#include "app.h"
#include <GL/gl.h>
#include <stdio.h>
#include <math.h>
#define _USE_MATH_DEFINES

void init_app(App* app){
    app->is_running = false;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    app->window = SDL_CreateWindow(
        "Shadow Hunter",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        dm.w, dm.h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

    if(app->window == NULL){
        printf("Unable to create the application window\n");
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if(app->gl_context == NULL){
        printf("Unable to create the OpenGL context\n");
        return;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);

    init_opengl();
    reshape(dm.w, dm.h);

    init_camera(&(app->camera));
    init_scene(&(app->scene));

    app->uptime = (double)SDL_GetTicks() / 1000.0;
    app->is_running = true;
}

void init_opengl(){
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);
    
    GLfloat bg_color[] = {0.05f, 0.05f, 0.08f, 1.0f};
    glClearColor(bg_color[0], bg_color[1], bg_color[2], bg_color[3]);

    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, bg_color);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, -3.0f);
    glFogf(GL_FOG_END, 30.0f);
    glHint(GL_FOG_HINT, GL_NICEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void reshape(int width, int height){
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double ratio = (double)width / (double)height;
    double fov = 60.0;
    double zNear = 0.1, zFar = 1000.0;
    double fH = tan(fov / 360.0 * M_PI) * zNear;
    double fW = fH * ratio;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void handle_app_events(App* app){
    SDL_Event event;

    while (SDL_PollEvent(&event)){
        switch (event.type){
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_ESCAPE:
                app->is_running = false;
                break;
            case SDL_SCANCODE_W:
                set_camera_speed(&(app->camera), 5.0);
                break;
            case SDL_SCANCODE_S:
                set_camera_speed(&(app->camera), -5.0);
                break;
            case SDL_SCANCODE_A:
                set_camera_side_speed(&(app->camera), 5.0);
                break;
            case SDL_SCANCODE_D:
                set_camera_side_speed(&(app->camera), -5.0);
                break;
            case SDL_SCANCODE_F:
                app->scene.flashlight_level++;
                if(app->scene.flashlight_level > 3) app->scene.flashlight_level = 0;
                
                if(app->scene.battery <= 0.0f) {
                    app->scene.flashlight_level = 0;
                }
                break;
            case SDL_SCANCODE_KP_PLUS:
            case SDL_SCANCODE_EQUALS:
                if(app->scene.battery > 0.0f){
                    app->scene.flashlight_level++;
                    if(app->scene.flashlight_level > 3) app->scene.flashlight_level = 3;
                }
                break;
            case SDL_SCANCODE_KP_MINUS:
            case SDL_SCANCODE_MINUS:
                app->scene.flashlight_level--;
                if(app->scene.flashlight_level < 0) app->scene.flashlight_level = 0;
                break;
            default:
                break;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.scancode){
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_S:
                set_camera_speed(&(app->camera), 0);
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
                set_camera_side_speed(&(app->camera), 0);
                break;
            default:
                break;
            }
            break;

        case SDL_MOUSEMOTION:
            rotate_camera(&(app->camera), event.motion.xrel * -0.1, event.motion.yrel * -0.1);
            break;

        case SDL_QUIT:
            app->is_running = false;
            break;
        default:
            break;
        }
    }
}

void update_app(App* app){
    double current_time = (double)SDL_GetTicks() / 1000.0;
    double elapsed_time = current_time - app->uptime;
    app->uptime = current_time;

    float old_x = app->camera.position.x;
    float old_y = app->camera.position.y;

    update_camera(&(app->camera), elapsed_time);
    update_scene(&(app->scene), elapsed_time);

    float new_x = app->camera.position.x;
    float new_y = app->camera.position.y;

    app->camera.position.x = new_x;
    app->camera.position.y = old_y;

    if(is_colliding(&(app->scene), app->camera.position.x, app->camera.position.y, PLAYER_RADIUS)){
        app->camera.position.x = old_x; 
    }

    app->camera.position.y = new_y;
    if(is_colliding(&(app->scene), app->camera.position.x, app->camera.position.y, PLAYER_RADIUS)){
        app->camera.position.y = old_y; 
    }

    float terrain_z = get_terrain_height(&(app->scene), app->camera.position.x, app->camera.position.y);
    app->camera.position.z = terrain_z + 1.5;
}

void render_app(App* app){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if(app->scene.flashlight_level > 0){
        glEnable(GL_LIGHT1);

        float pos[] = {-0.3f, -0.2f, 0.0f, 1.0f};
        float dir[] = {0.0f, 0.0f, -1.0f};

        glLightfv(GL_LIGHT1, GL_POSITION, pos);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir);

        float diff[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
        glLightfv(GL_LIGHT1, GL_SPECULAR, diff);
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 50.0f);
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);

        if(app->scene.flashlight_level == 1){
            glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 25.0f);
            glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
            glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);
        }else if(app->scene.flashlight_level == 2){ 
            glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20.0f);
            glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.02f);
            glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.002f);
        }else if(app->scene.flashlight_level == 3){ 
            glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 15.0f);
            glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.005f);
            glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0005f);
        }
    }else{
        glDisable(GL_LIGHT1);
    }

    glPushMatrix();
    set_view(&(app->camera));
    render_scene(&(app->scene));
    glPopMatrix();

    int w, h;
    SDL_GetWindowSize(app->window, &w, &h);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)w, 0.0, (double)h, -1.0, 1.0); 

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST); 
    glDisable(GL_LIGHTING);   
    glEnable(GL_ALPHA_TEST);  
    glAlphaFunc(GL_GREATER, 0.1f);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, app->scene.hud_flashlight_texture);

    float icon_size = h * 0.5f;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(icon_size, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(icon_size, icon_size);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, icon_size);
    glEnd();

    glDisable(GL_ALPHA_TEST); 
    glEnable(GL_LIGHTING);    
    glEnable(GL_DEPTH_TEST);  

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App* app){
    if(app->gl_context != NULL){
        SDL_GL_DeleteContext(app->gl_context);
    }

    if(app->window != NULL){
        SDL_DestroyWindow(app->window);
    }
    SDL_Quit();
}
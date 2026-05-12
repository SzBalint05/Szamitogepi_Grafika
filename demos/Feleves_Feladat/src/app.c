#include "app.h"
#include "physics.h"
#include "ui.h"
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

    app->help_texture = load_texture("assets/textures/help.png");
    app->game_over_texture = load_texture("assets/textures/gameover.png");

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

    // Köd
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, bg_color);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, -5.0f);
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
    double zNear = 0.1, zFar = 200.0;
    double fH = tan(fov / 360.0 * M_PI) * zNear;
    double fW = fH * ratio;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void handle_app_events(App* app){
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch(event.type){
        case SDL_KEYDOWN:
            switch(event.key.keysym.scancode){
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
                if(!app->is_paused && !app->is_game_over){
                    app->scene.flashlight_level++;
                    if(app->scene.flashlight_level > 3) app->scene.flashlight_level = 0;
                    
                    if(app->scene.battery <= 0.0f) {
                        app->scene.flashlight_level = 0;
                    }
                }
                break;
            case SDL_SCANCODE_SPACE:
                if(!app->camera.is_jumping && !app->is_paused && !app->is_game_over){
                    app->camera.is_jumping = true;
                    app->camera.vertical_speed = 6.0f;
                }
                break;
            case SDL_SCANCODE_F1:
                app->is_paused = !app->is_paused;
                break;
            default:
                break;
            }
            break;

        case SDL_KEYUP:
            switch(event.key.keysym.scancode){
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

        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == SDL_BUTTON_LEFT && !app->is_paused && !app->is_game_over){
                shoot_weapon(&(app->scene), &(app->camera));
            }
            break;

        case SDL_MOUSEMOTION:
            if(!app->is_paused && !app->is_game_over){
                rotate_camera(&(app->camera), event.motion.xrel * -0.05, event.motion.yrel * -0.05);
            }
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

    if(app->is_game_over){
        app->game_over_timer -= elapsed_time;
        if(app->game_over_timer <= 0.0f){
            init_camera(&(app->camera));
            app->scene.hp = 100;
            app->scene.battery = 100.0f;
            app->scene.kills = 0;
            app->scene.flashlight_level = 0;
            app->scene.is_shooting = false;
            app->scene.damage_indicator_timer = 0.0f;
            for(int i = 0; i < MAX_GHOSTS; i++) app->scene.ghosts[i].is_alive = false;
            for(int i = 0; i < 3; i++) spawn_ghost(&(app->scene), 64.0f, 64.0f);

            app->is_game_over = false;
        }
        return;
    }

    if (app->is_paused) return;

    // Előző pozíció lementése
    float old_x = app->camera.position.x;
    float old_y = app->camera.position.y;

    update_camera(&(app->camera), elapsed_time);
    update_scene(&(app->scene), elapsed_time, &(app->camera)); 

    if(app->scene.hp <= 0 && !app->is_game_over){
        app->is_game_over = true;
        app->game_over_timer = 3.0f;
    }

    // Fizika és Ütközésvizsgálat
    apply_physics(&(app->camera), &(app->scene), old_x, old_y, elapsed_time);
}

void render_app(App* app){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int w, h;
    SDL_GetWindowSize(app->window, &w, &h);
    float ratio = (float)w / (float)h;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Zseblámpa fényének beállítása
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

    // Lövés vonal
    if(app->scene.is_shooting){
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glLineWidth(3.0f);
        glColor3f(1.0f, 0.8f, 0.0f);

        float start_x = 0.27f * ratio;
        float start_y = -0.58f + (0.27f * ratio);

        glBegin(GL_LINES);
            glVertex3f(start_x, start_y, -1.0f);
            glVertex3f(0.0f, 0.0f, -50.0f);
        glEnd();

        glEnable(GL_LIGHTING);
    }

    // 3D-s Kamera és Világ kirajzolása
    glPushMatrix();
    set_view(&(app->camera));
    render_scene(&(app->scene));
    glPopMatrix();

    // HUD kirajzolása
    render_hud(&(app->scene), w, h, app->is_paused, app->is_game_over, app->help_texture, app->game_over_texture);

    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App* app){
    glDeleteTextures(1, &(app->help_texture));
    glDeleteTextures(1, &(app->game_over_texture));

    destroy_scene(&(app->scene));

    if(app->gl_context != NULL){
        SDL_GL_DeleteContext(app->gl_context);
    }

    if(app->window != NULL){
        SDL_DestroyWindow(app->window);
    }
    SDL_Quit();
}
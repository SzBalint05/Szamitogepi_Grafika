#include "app.h"
#include <GL/gl.h>
#include <stdio.h>
#include <math.h>
#define _USE_MATH_DEFINES

void init_app(App* app){
    app->is_running = false;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
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

    if (app->window == NULL){
        printf("Unable to create the application window!\n");
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if (app->gl_context == NULL){
        printf("Unable to create the OpenGL context!\n");
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
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
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

    update_camera(&(app->camera), elapsed_time);
    update_scene(&(app->scene), elapsed_time);
}

void render_app(App* app){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    set_view(&(app->camera));
    render_scene(&(app->scene));
    glPopMatrix();

    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App* app){
    if (app->gl_context != NULL){
        SDL_GL_DeleteContext(app->gl_context);
    }

    if (app->window != NULL){
        SDL_DestroyWindow(app->window);
    }
    SDL_Quit();
}
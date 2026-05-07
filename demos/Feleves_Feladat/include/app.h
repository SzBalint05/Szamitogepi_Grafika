#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "camera.h"
#include "scene.h"

typedef struct{
    SDL_Window* window;
    SDL_GLContext gl_context;
    bool is_running;
    double uptime;
    Camera camera;
    Scene scene;
}App;

void init_app(App* app);
void init_opengl();
void reshape(int width, int height);
void handle_app_events(App* app);
void update_app(App* app);
void render_app(App* app);
void destroy_app(App* app);

#endif // APP_H
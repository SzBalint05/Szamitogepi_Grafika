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

    app->help_texture = load_texture("assets/textures/help.png");

    app->is_game_over = false;
    app->game_over_timer = 0.0f;
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
    double zNear = 0.1, zFar = 1000.0;
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
                if(!app->is_paused){
                    app->scene.flashlight_level++;
                    if(app->scene.flashlight_level > 3) app->scene.flashlight_level = 0;
                    
                    if(app->scene.battery <= 0.0f) {
                        app->scene.flashlight_level = 0;
                    }
                }
                break;
            case SDL_SCANCODE_SPACE:
                if(!app->camera.is_jumping && !app->is_paused){
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
            if(event.button.button == SDL_BUTTON_LEFT){
                if(!app->scene.is_shooting && !app->is_paused && app->scene.gun_cooldown <= 0.0f){
                    app->scene.is_shooting = true;
                    app->scene.shot_timer = 0.05f;
                    app->scene.gun_cooldown = 0.5f;

                    float dir_x = cos(degree_to_radian(app->camera.rotation.z)) * cos(degree_to_radian(app->camera.rotation.x));
                    float dir_y = sin(degree_to_radian(app->camera.rotation.z)) * cos(degree_to_radian(app->camera.rotation.x));
                    float dir_z = sin(degree_to_radian(app->camera.rotation.x));

                    for(int i = 0; i < MAX_GHOSTS; i++){
                        if(app->scene.ghosts[i].is_alive && app->scene.ghosts[i].is_illuminated && !app->scene.ghosts[i].is_dying){
                            
                            float vx = app->scene.ghosts[i].x - app->camera.position.x;
                            float vy = app->scene.ghosts[i].y - app->camera.position.y;
                            float vz = app->scene.ghosts[i].z - app->camera.position.z;
                            float t = vx*dir_x + vy*dir_y + vz*dir_z; 

                            if(t > 0.0f && t < 50.0f){
                                float hit_x = app->camera.position.x + dir_x * t;
                                float hit_y = app->camera.position.y + dir_y * t;
                                float hit_z = app->camera.position.z + dir_z * t;
                                float dist_sq = pow(app->scene.ghosts[i].x - hit_x, 2) +  pow(app->scene.ghosts[i].y - hit_y, 2) + pow(app->scene.ghosts[i].z - hit_z, 2);

                                if(dist_sq < 4.0f){
                                    app->scene.ghosts[i].hp--; 
                                    if(app->scene.ghosts[i].hp <= 0){
                                        app->scene.ghosts[i].is_dying = true;
                                        app->scene.ghosts[i].alpha = 1.0f;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            break;

        case SDL_MOUSEMOTION:
            if(!app->is_paused){
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

    float old_x = app->camera.position.x;
    float old_y = app->camera.position.y;
    float current_z = app->camera.position.z - 1.5f;

    update_camera(&(app->camera), elapsed_time);
    update_scene(&(app->scene), elapsed_time, &(app->camera)); 

    if(app->scene.hp <= 0 && !app->is_game_over){
        app->is_game_over = true;
        app->game_over_timer = 3.0f;
    }

    float new_x = app->camera.position.x;
    float new_y = app->camera.position.y;

    app->camera.position.x = new_x;
    app->camera.position.y = old_y;
    if(is_colliding(&(app->scene), app->camera.position.x, app->camera.position.y, current_z, PLAYER_RADIUS)){
        app->camera.position.x = old_x; 
    }

    app->camera.position.y = new_y;
    if(is_colliding(&(app->scene), app->camera.position.x, app->camera.position.y, current_z, PLAYER_RADIUS)){
        app->camera.position.y = old_y; 
    }

    float target_z = get_surface_height(&(app->scene), app->camera.position.x, app->camera.position.y);

    if(app->camera.is_jumping){
        current_z += app->camera.vertical_speed * elapsed_time;
        app->camera.vertical_speed -= 15.0f * elapsed_time;

        if(current_z <= target_z){
            current_z = target_z;
            app->camera.is_jumping = false;
            app->camera.vertical_speed = 0.0f;
        }
    }else{
        if (current_z > target_z + 0.3f){
            app->camera.is_jumping = true;
            app->camera.vertical_speed = 0.0f;
        }else{
            current_z = target_z;
        }
    }
    app->camera.position.z = current_z + 1.5f; 
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

    // Kamera kirajzolása
    glPushMatrix();
    set_view(&(app->camera));
    render_scene(&(app->scene));
    glPopMatrix();

    // HUD
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)w, 0.0, (double)h, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST); 
    glDisable(GL_LIGHTING);   
    glDisable(GL_TEXTURE_2D);

    // Célkereszt
    glPointSize(4.0f);
    glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(w / 2.0f, h / 2.0f);
    glEnd();

    // HP és töltési csík
    float bar_width = 500.0f;
    float bar_height = 25.0f;
    float start_x = (w - bar_width) / 2.0f;

    // HP csík
    glColor3f(0.2f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(start_x, 20); glVertex2f(start_x + bar_width, 20);
        glVertex2f(start_x + bar_width, 20 + bar_height); glVertex2f(start_x, 20 + bar_height);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);
    float hp_w = (app->scene.hp / 100.0f) * bar_width;
    glBegin(GL_QUADS);
        glVertex2f(start_x, 20); glVertex2f(start_x + hp_w, 20);
        glVertex2f(start_x + hp_w, 20 + bar_height); glVertex2f(start_x, 20 + bar_height);
    glEnd();

    // Töltési csík
    glColor3f(0.1f, 0.1f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(start_x, 60); glVertex2f(start_x + bar_width, 60);
        glVertex2f(start_x + bar_width, 60 + bar_height); glVertex2f(start_x, 60 + bar_height);
    glEnd();

    glColor3f(0.8f, 0.8f, 0.0f);
    float bat_w = (app->scene.battery / 100.0f) * bar_width;
    glBegin(GL_QUADS);
        glVertex2f(start_x, 60); glVertex2f(start_x + bat_w, 60);
        glVertex2f(start_x + bat_w, 60 + bar_height); glVertex2f(start_x, 60 + bar_height);
    glEnd();

    // Scoreboard
    glLineWidth(10.0f);
    glColor3f(1.0f, 0.0f, 0.0f);

    float line_height = 80.0f;
    float spacing = 25.0f;
    float tally_x = 50.0f;
    float tally_y = h - 120.0f;

    for(int i = 0; i < app->scene.kills; i++){
        int group_pos = i % 5;

        if(group_pos < 4){
            glBegin(GL_LINES);
                glVertex2f(tally_x, tally_y);
                glVertex2f(tally_x, tally_y + line_height);
            glEnd();
            tally_x += spacing;
        }else{
            float start_x = tally_x - (spacing * 4.5f);
            float start_y = tally_y - (line_height * 0.1f);
            float end_x = tally_x - (spacing * 0.5f);
            float end_y = tally_y + (line_height * 1.1f);

            glBegin(GL_LINES);
                glVertex2f(start_x, start_y);
                glVertex2f(end_x, end_y);
            glEnd();
            tally_x += spacing * 2.0f;
        }
    }

    // Zseblámpa ikon
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f);
    glBindTexture(GL_TEXTURE_2D, app->scene.hud_flashlight_texture);
    glColor3f(1.0f, 1.0f, 1.0f);


    float icon_s = w * 0.26f;
    
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(icon_s, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(icon_s, icon_s);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, icon_s);
    glEnd();

    // Fegyver ikon
    glBindTexture(GL_TEXTURE_2D, app->scene.gun_texture);

    float gun_s = w * 0.30f;
    float gun_x = w - gun_s;
    float gun_y = 0.0f;

    if(app->scene.is_shooting){
        gun_x += w * 0.02f;
        gun_y -= h * 0.03f;
    }

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(gun_x, gun_y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(gun_x + gun_s, gun_y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(gun_x + gun_s, gun_y + gun_s);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(gun_x, gun_y + gun_s);
    glEnd();

    // Sebzés indikátor
    if(app->scene.damage_indicator_timer > 0.0f){
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.0f, 0.0f, 0.3f);
        
        float border_w = w * 0.03f;

        glBegin(GL_QUADS);
            glVertex2f(0, 0); glVertex2f(w, 0); glVertex2f(w, border_w); glVertex2f(0, border_w);
            glVertex2f(0, h - border_w); glVertex2f(w, h - border_w); glVertex2f(w, h); glVertex2f(0, h);
            glVertex2f(0, border_w); glVertex2f(border_w, border_w); glVertex2f(border_w, h - border_w); glVertex2f(0, h - border_w);
            glVertex2f(w - border_w, border_w); glVertex2f(w, border_w); glVertex2f(w, h - border_w); glVertex2f(w - border_w, h - border_w);
        glEnd();
        glDisable(GL_BLEND);
    }

    // Game Over képernyő
    if(app->is_game_over){
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        
        glBegin(GL_QUADS);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(w, 0.0f);
            glVertex2f(w, h);
            glVertex2f(0.0f, h);
        glEnd();
        glDisable(GL_BLEND);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_ALPHA_TEST);
        glBindTexture(GL_TEXTURE_2D, app->game_over_texture);
        glColor3f(1.0f, 1.0f, 1.0f);

        float help_w = w * 0.5f;
        float help_h = help_w * 0.75f;
        float help_x = (w - help_w) / 2.0f;
        float help_y = (h - help_h) / 2.0f;

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(help_x, help_y);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(help_x + help_w, help_y);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(help_x + help_w, help_y + help_h);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(help_x, help_y + help_h);
        glEnd();
    }

    // Paused képernyő
    if(app->is_paused){
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        
        glBegin(GL_QUADS);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(w, 0.0f);
            glVertex2f(w, h);
            glVertex2f(0.0f, h);
        glEnd();
        glDisable(GL_BLEND);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_ALPHA_TEST);
        glBindTexture(GL_TEXTURE_2D, app->help_texture);
        glColor3f(1.0f, 1.0f, 1.0f);

        float help_w = w * 0.5f;
        float help_h = help_w * 0.75f;
        float help_x = (w - help_w) / 2.0f;
        float help_y = (h - help_h) / 2.0f;

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(help_x, help_y);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(help_x + help_w, help_y);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(help_x + help_w, help_y + help_h);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(help_x, help_y + help_h);
        glEnd();
    }

    // Visszaállítás
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
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
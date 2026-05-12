#include "ui.h"

void draw_bar(float x, float y, float width, float height, float percentage, Color bg, Color fg){
    glColor3f(bg.red, bg.green, bg.blue);
    glBegin(GL_QUADS);
        glVertex2f(x, y); glVertex2f(x + width, y);
        glVertex2f(x + width, y + height); glVertex2f(x, y + height);
    glEnd();
    
    glColor3f(fg.red, fg.green, fg.blue);
    float fill_w = percentage * width;
    glBegin(GL_QUADS);
        glVertex2f(x, y); glVertex2f(x + fill_w, y);
        glVertex2f(x + fill_w, y + height); glVertex2f(x, y + height);
    glEnd();
}

void draw_fullscreen_overlay(int w, int h, GLuint texture){
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f); glVertex2f(w, 0.0f); 
        glVertex2f(w, h); glVertex2f(0.0f, h);
    glEnd();
    glDisable(GL_BLEND);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(1.0f, 1.0f, 1.0f);

    float img_w = w * 0.5f;
    float img_h = img_w * 0.75f;
    float img_x = (w - img_w) / 2.0f;
    float img_y = (h - img_h) / 2.0f;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(img_x, img_y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(img_x + img_w, img_y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(img_x + img_w, img_y + img_h);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(img_x, img_y + img_h);
    glEnd();
}

void draw_crosshair(int w, int h){
    glPointSize(4.0f);
    glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(w / 2.0f, h / 2.0f);
    glEnd();
}

void draw_status_bars(const Scene* scene, int w){
    float bar_width = 500.0f;
    float bar_height = 25.0f;
    float start_x = (w - bar_width) / 2.0f;

    draw_bar(start_x, 20.0f, bar_width, bar_height, scene->hp / 100.0f, (Color){0.2f, 0.0f, 0.0f}, (Color){1.0f, 0.0f, 0.0f});
    draw_bar(start_x, 60.0f, bar_width, bar_height, scene->battery / 100.0f, (Color){0.1f, 0.1f, 0.0f}, (Color){0.8f, 0.8f, 0.0f});
}

void draw_scoreboard(int kills, int h){
    glLineWidth(10.0f);
    glColor3f(1.0f, 0.0f, 0.0f);

    float line_height = 80.0f;
    float spacing = 25.0f;
    float tally_x = 50.0f;
    float tally_y = h - 120.0f;

    for(int i = 0; i < kills; i++){
        int group_pos = i % 5;

        if(group_pos < 4){
            glBegin(GL_LINES);
                glVertex2f(tally_x, tally_y);
                glVertex2f(tally_x, tally_y + line_height);
            glEnd();
            tally_x += spacing;
        }else{
            float start_x_val = tally_x - (spacing * 4.5f);
            float start_y_val = tally_y - (line_height * 0.1f);
            float end_x = tally_x - (spacing * 0.5f);
            float end_y = tally_y + (line_height * 1.1f);

            glBegin(GL_LINES);
                glVertex2f(start_x_val, start_y_val);
                glVertex2f(end_x, end_y);
            glEnd();
            tally_x += spacing * 2.0f;
        }
    }
}

void draw_flashlight_icon(GLuint texture, int w){
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(1.0f, 1.0f, 1.0f);

    float icon_s = w * 0.26f;
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(icon_s, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(icon_s, icon_s);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, icon_s);
    glEnd();
}

void draw_gun_icon(GLuint texture, bool is_shooting, int w, int h){
    glBindTexture(GL_TEXTURE_2D, texture);
    float gun_s = w * 0.30f;
    float gun_x = w - gun_s;
    float gun_y = 0.0f;

    if(is_shooting){
        gun_x += w * 0.02f;
        gun_y -= h * 0.03f;
    }

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(gun_x, gun_y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(gun_x + gun_s, gun_y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(gun_x + gun_s, gun_y + gun_s);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(gun_x, gun_y + gun_s);
    glEnd();
}

void draw_damage_indicator(float timer, int w, int h){
    if(timer > 0.0f){
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
}

void render_hud(const Scene* scene, int w, int h, bool is_paused, bool is_game_over, GLuint help_tex, GLuint go_tex) {
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

    draw_crosshair(w, h);
    draw_status_bars(scene, w);
    draw_scoreboard(scene->kills, h);
    draw_flashlight_icon(scene->hud_flashlight_texture, w);
    draw_gun_icon(scene->gun_texture, scene->is_shooting, w, h);
    draw_damage_indicator(scene->damage_indicator_timer, w, h);

    if(is_game_over){
        draw_fullscreen_overlay(w, h, go_tex);
    }
    if(is_paused){
        draw_fullscreen_overlay(w, h, help_tex);
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
}
#ifndef UI_H
#define UI_H

#include "scene.h"
#include "utils.h"
#include <stdbool.h>
#include <GL/gl.h>

void draw_bar(float x, float y, float width, float height, float percentage, Color bg, Color fg);
void draw_fullscreen_overlay(int w, int h, GLuint texture);
void draw_crosshair(int w, int h);
void draw_status_bars(const Scene* scene, int w);
void draw_scoreboard(int kills, int h);
void draw_flashlight_icon(GLuint texture, int w);
void draw_gun_icon(GLuint texture, bool is_shooting, int w, int h);
void draw_damage_indicator(float timer, int w, int h);

void render_hud(const Scene* scene, int w, int h, bool is_paused, bool is_game_over, GLuint help_tex, GLuint go_tex);

#endif // UI_H
#ifndef GHOST_H
#define GHOST_H

#include <stdbool.h>

#define MAX_GHOSTS 6

typedef struct Ghost{
    float x;
    float y;
    float z;
    bool is_alive;
    bool is_illuminated;
    bool is_dying;
    int hp;
    float scale;
    float alpha;
    bool killed_by_player;
    float damage_timer;
}Ghost;

struct Scene;
struct Camera;

void spawn_ghost(struct Scene* scene, float player_x, float player_y);
void update_ghosts(struct Scene* scene, double time, const struct Camera* camera);
void render_ghosts(const struct Scene* scene);
void check_ghost_hits(struct Scene* scene, const struct Camera* camera, float dir_x, float dir_y, float dir_z);

#endif // GHOST_H
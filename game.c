// If I can't use all of this with c++ I think I'd rather just write all the damn vectors by hand
// and stick to c99. Fuck c++.

// @TODO: Why doesn't inline work?

// I think a much larger map would really help a lot.

// TODO List
// Port sweep_aabb to the inline spot I set up.

// Start iterating on some real gameplay.
// Move steve.h stuff into this file too.

// @NOTE: This is c++20+ only
// Because I use designated initializers (c99) AND operator overloading (c++).
// It's really just c99 + operator overloading but that's only c++20 for some reason.

// Lets do a large world and a level editor.

// Ok so the level editing stuff sucks, also using tiled suckes and using an external editor sucks.
// I'm not sure how I wanna design these worlds.
// I do wanna do precedural stuff eventuially so maybe I just work on that, do the runtime data structure and fill
// it in with code from the beginning.

// Guess I want some sort of sparse storage thing to get started with.

// The tilemap does work.

// TODO: Just inline whatever you do use. Easier to share the code.
#include "steve.h"
#include "steve.c"

typedef struct {
    int x, y;
} I2;

typedef struct {
    int x, y, z;
} I3;

typedef struct {
    float x, y;
} F2;

F2 f2(float x, float y) { return (F2){.x = x, .y = y}; }

const F2 f2_zero = {.x = 0.0f, .y = 0.0f};
const F2 f2_one = {.x = 1.0f, .y = 1.0f};
const F2 f2_x = {.x = 1.0f, .y = 0.0f};
const F2 f2_y = {.x = 0.0f, .y = 1.0f};

typedef struct {
    float x, y, z;
} F3;

F3 f3(float x, float y, float z) { return (F3){.x = x, .y = y, .z = z}; }

const F3 f3_zero = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
const F3 f3_one = {.x = 1.0f, .y = 1.0f, .z = 1.0f};
const F3 f3_x = {.x = 1.0f, .y = 0.0f, .z = 0.0f};
const F3 f3_y = {.x = 0.0f, .y = 1.0f, .z = 0.0f};
const F3 f3_z = {.x = 0.0f, .y = 0.0f, .z = 1.0f};

F3 f2_xyz(F2 v) { return (F3){.x = v.x, .y = v.y, .z = 0.0}; }
F2 f3_xy(F3 v) { return (F2){.x = v.x, .y = v.y}; }

F2 f2_add(F2 a, F2 b) { return (F2){.x = a.x + b.x, .y = a.y + b.y}; }
F2 f2_sub(F2 a, F2 b) { return (F2){.x = a.x - b.x, .y = a.y - b.y}; }
F2 f2_mul_f(F2 v, float f) { return (F2){.x = v.x * f, .y = v.y * f}; }
F2 f2_div_f(F2 v, float f) { return (F2){.x = v.x / f, .y = v.y / f}; }
F2 f2_neg(F2 v) { return (F2){.x = -v.x, .y = -v.y}; }
float f2_mag2(F2 v) { return v.x * v.x + v.y * v.y; }
float f2_mag(F2 v) {
    return sqrtf(f2_mag2(v));
}
F2 f2_normalize(F2 v) {
    float m = f2_mag(v);
    if (m != 0.0) {
        return f2_div_f(v, m);
    } else {
        return v;
    }
}

F3 f3_add(F3 a, F3 b) { return (F3){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z}; }
F3 f3_sub(F3 a, F3 b) { return (F3){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z}; }
F3 f3_mul_f(F3 v, float f) { return (F3){.x = v.x * f, .y = v.y * f, .z = v.z * f}; }
F3 f3_div_f(F3 v, float f) { return (F3){.x = v.x / f, .y = v.y / f, .z = v.z / f}; }
F3 f3_neg(F3 v) { return (F3){.x = -v.x, .y = -v.y, .z = -v.z}; }
float f3_mag2(F3 v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
float f3_mag(F3 v) {
    return sqrtf(f3_mag2(v));
}
F3 f3_normalize(F3 v) {
    float m = f3_mag(v);
    if (m != 0.0) {
        return f3_div_f(v, m);
    } else {
        return v;
    }
}

// Debug State
// Because of the setup of this project I can just write to this anywhere and render it.
// Super useful
char* text_buf = NULL;
int frame_ticks = 0;

// Called every frame, reset stuff you want to reset every frame here.
void reset_debug() {
    arrclear(text_buf);
    frame_ticks = 0;
}

#include "tilemap.h"

typedef enum {
    TILE_NONE,
    TILE_FLOOR,
    TILE_GRASS,
    TILE_DIRT,
    TILE_ROAD,
    TILE_BUSH,
    TILE_WALL,
    TILE_ROCK,
    TILE_SHALLOW_WATER,
    TILE_DEEP_WATER,
    NUM_TILE_KINDS,
} TileKind;

static char tile_chars[NUM_TILE_KINDS] = {
    [TILE_NONE] = ' ',
    [TILE_FLOOR] = 'f',
    [TILE_GRASS] = 'g',
    [TILE_DIRT] = 'x',
    [TILE_ROAD] = 'r',
    [TILE_BUSH] = 'B',
    [TILE_WALL] = 'W',
    [TILE_ROCK] = 'R',
    [TILE_SHALLOW_WATER] = 'o',
    [TILE_DEEP_WATER] = 'O',
};

TileKind tile_from_char(char c) {
    for (int i=0; i<NUM_TILE_KINDS; i++) {
        if (tile_chars[i] == c) {
            return (TileKind)i;
        }
    }
    return TILE_NONE;
}

char map_i(size_t x, size_t y) {
    return TILE_MAP[((TILE_MAP_DEPTH - 1 - y) * TILE_MAP_WIDTH) + x];
}

// Platform layer types
typedef struct {
    bool down;
    //bool pressed;
    //bool released;
} DigitalButton;

typedef struct {
    DigitalButton w;
    DigitalButton a;
    DigitalButton s;
    DigitalButton d;
} Input;

// Platform layer state
Input input = {0};
double t = 0;

// Simulation types
typedef struct {
    bool hit;
    float hit_time;
    F2 hit_normal;
} SweepResult;

typedef struct {
    F2 center;
    F2 extent;
} Aabb;

// @OPTIMIZE: This is like the inner loop, it probably has to happen on a bunch of shit at once.
SweepResult sweep_aabb(Aabb a, Aabb b, F2 ray, float dt) {
    bool hit = false;
    float hit_time = HUGE_VAL;
    F2 hit_normal = f2_zero;
    F2 minkowski_extent = f2_add(a.extent, b.extent);
    F2 min = f2_sub(b.center, minkowski_extent);
    F2 max = f2_add(b.center, minkowski_extent);
    F2 o = a.center;
    // tile top
    if (ray.y < 0.0) {
        float hit_t = (max.y - o.y) / ray.y;
        if (hit_t > 0.0 && hit_t < dt) {
            float x = o.x + ray.x * hit_t;
            if (x >= min.x && x <= max.x) {
                if (hit_t < hit_time) {
                    hit = true;
                    hit_time = hit_t;
                    hit_normal = f2_y;
                }
            }
        }
    }
    // tile bottom
    if (ray.y > 0.0) {
        float hit_t = (min.y - o.y) / ray.y;
        if (hit_t > 0.0 && hit_t < dt) {
            float x = o.x + ray.x * hit_t;
            if (x >= min.x && x <= max.x) {
                if (hit_t < hit_time) {
                    hit = true;
                    hit_time = hit_t;
                    hit_normal = f2_neg(f2_y);
                }
            }
        }
    }
    // tile left
    if (ray.x > 0.0) {
        float hit_t = (min.x - o.x) / ray.x;
        if (hit_t > 0.0 && hit_t < dt) {
            float y = o.y + ray.y * hit_t;
            if (y >= min.y && y <= max.y) {
                if (hit_t < hit_time) {
                    hit = true;
                    hit_time = hit_t;
                    hit_normal = f2_neg(f2_x);
                }
            }
        }
    }
    // tile right
    if (ray.x < 0.0) {
        float hit_t = (max.x - o.x) / ray.x;
        if (hit_t > 0.0 && hit_t < dt) {
            float y = o.y + ray.y * hit_t;
            if (y >= min.y && y <= max.y) {
                if (hit_t < hit_time) {
                    hit = true;
                    hit_time = hit_t;
                    hit_normal = f2_x;
                }
            }
        }
    }
    return (SweepResult){.hit = hit, .hit_time = hit_time, .hit_normal = hit_normal};
}

typedef enum {
    PLAYER,
    BADGUY,
} EntityKind;

typedef struct {
    EntityKind kind;
    F3 pos;
    F3 dp;
    F3 ddp;

    F3 target;
} Entity;

typedef enum {
    COLLIDE_ENTITY,
    COLLIDE_TILE,
} CollisionKind;

typedef struct {
    CollisionKind kind;
    size_t entity;
    union {
        size_t hit_entity;
        F3 hit_tile;
    };
    F2 hit_plane; // @TODO: Something better than this.
} Collision;

const bool tile_collides[NUM_TILE_KINDS] = {
    [TILE_NONE] = false,
    [TILE_FLOOR] = false,
    [TILE_GRASS] = false,
    [TILE_DIRT] = false,
    [TILE_ROAD] = false,
    [TILE_BUSH] = true,
    [TILE_WALL] = true,
    [TILE_ROCK] = true,
    [TILE_SHALLOW_WATER] = false,
    [TILE_DEEP_WATER] = true,
};

// @TODO: tile_geometries or something.

// Simulation constants
const float SIM_DT = 1.0f / 60.0f;
const float PLAYER_MAX_SPEED = 5.5f;
const float PLAYER_ACCELERATION = 15.0f;
const float BADGUY_MAX_SPEED = 5.0f;
const float BADGUY_ACCELERATION = 10.0f;
const float FRICTION = 4.0f;

// Simulation state
double sim_time = 0.0;
Entity *entities = NULL;
size_t player = 0;
int health = 5;
bool dead = false;

void game_setup() {
    arrpush(entities, (Entity){
            .kind = PLAYER,
            .pos = f3(48.0, 53.0, 0.0),
    });
    arrpush(entities, (Entity){
            .kind = BADGUY,
            .pos = f3(31.0, 35.0, 0.0),
    });
    health = 10;
}

// Frame simulation state
Collision *collisions = NULL;

void sim_tick() {
    arrclear(collisions);

    F3 player_accel = {0};
    if (input.w.down) { player_accel = f3_add(player_accel, f3_y); }
    if (input.a.down) { player_accel = f3_sub(player_accel, f3_x); }
    if (input.s.down) { player_accel = f3_sub(player_accel, f3_y); }
    if (input.d.down) { player_accel = f3_add(player_accel, f3_x); }

    player_accel = f3_normalize(player_accel);

    for (int entity_i=0; entity_i < arrlen(entities); entity_i++) {
        Entity entity = entities[entity_i];
        float max_speed = 0.0;
        switch(entity.kind) {
            case PLAYER: {
                entity.ddp = player_accel;
                entity.ddp = f3_mul_f(entity.ddp, PLAYER_ACCELERATION);
                max_speed = PLAYER_MAX_SPEED;
            } break;
            case BADGUY: {
                entity.target = entities[player].pos;
                entity.ddp = f3_normalize(f3_sub(entity.target, entity.pos));
                entity.ddp = f3_mul_f(entity.ddp, BADGUY_ACCELERATION);
                max_speed = BADGUY_MAX_SPEED;
            } break;
        }

        F3 new_dp = f3_add(f3_mul_f(entity.ddp, SIM_DT), entity.dp);
        F3 friction = f3_mul_f(f3_neg(new_dp), FRICTION);
        new_dp = f3_add(new_dp, f3_mul_f(friction, SIM_DT));

        if (f3_mag2(new_dp) > max_speed * max_speed) {
            new_dp = f3_mul_f(f3_normalize(new_dp), max_speed);
        }
        entity.dp = new_dp;

        F3 new_pos = f3_mul_f(entity.ddp, 0.5 * (SIM_DT * SIM_DT));
        new_pos = f3_add(new_pos, f3_mul_f(entity.dp, SIM_DT));
        new_pos = f3_add(new_pos, entity.pos);

        F2 ray = f3_xy(f3_sub(new_pos, entity.pos));
        if (f2_mag2(ray) > 0.0) {
            float m = f2_mag(ray) / SIM_DT;
            ray = f2_mul_f(f2_normalize(ray), m);
        }

        Aabb entity_geometry = (Aabb) {
                .center = f3_xy(entity.pos),
                .extent = f2(0.25, 0.25),
        };

        float dt_rem = SIM_DT;
        while (dt_rem > 0.0) {
            float min_hit_t = HUGE_VAL;
            F2 hit_plane = f2_zero;
            Collision collision = {0};

            for (int y=MAX(entity.pos.y-1, 0); y<MIN(entity.pos.y+1,TILE_MAP_DEPTH); y++) {
                for (int x=MAX(entity.pos.x-1, 0); x<MIN(entity.pos.x+1,TILE_MAP_WIDTH); x++) {
                    char tile_char = map_i(x, y);
                    TileKind tile_kind = tile_from_char(tile_char);

                    if (!tile_collides[tile_kind]) { continue; }
                    Aabb tile_geometry = (Aabb){
                            .center = f2((float)x, (float)y),
                            .extent = f2(0.5, 0.5),
                    };
                    SweepResult result = sweep_aabb(entity_geometry, tile_geometry, ray, dt_rem);
                    if (result.hit && result.hit_time < min_hit_t) {
                        min_hit_t = result.hit_time;
                        hit_plane = result.hit_normal;
                        collision = (Collision){
                                .entity = (size_t)entity_i,
                                .kind = COLLIDE_TILE,
                                // @TODO: Maybe make a u3 vector for this.
                                .hit_tile = f3((float)x,(float)y,(float)0),
                                .hit_plane = hit_plane,
                        };
                    }
                }
            }

            // Check collisions with other entities.
            for (int other_i=0; other_i<arrlen(entities); other_i++) {
                if (other_i == entity_i) { continue; }
                Entity other = entities[other_i];
                Aabb other_geometry = (Aabb){
                        .center = f3_xy(other.pos),
                        .extent = f2(0.25, 0.25),
                };
                SweepResult result = sweep_aabb(entity_geometry, other_geometry, ray, dt_rem);
                if (result.hit && result.hit_time < min_hit_t) {
                    min_hit_t = result.hit_time;
                    hit_plane = result.hit_normal;
                    collision = (Collision){
                            .entity = (size_t)entity_i,
                            .kind = COLLIDE_ENTITY,
                            .hit_entity = (size_t)other_i,
                            .hit_plane = hit_plane,
                    };
                }
            }
            
            F3 moved_from = entity.pos;

            if (min_hit_t < HUGE_VAL) {
                entity.pos = f3_add(entity.pos, f3_mul_f(f2_xyz(ray), (float)(min_hit_t - 0.0001)));
                dt_rem -= min_hit_t;
                if (hit_plane.x != 0.0) {
                    entity.dp.x = 0.0;
                    entity.ddp.x = 0.0;
                }
                if (hit_plane.y != 0.0) {
                    entity.dp.y = 0.0;
                    entity.ddp.y = 0.0;
                }

                arrpush(collisions, collision);

                new_pos = f3_mul_f(entity.ddp, 0.5 * (dt_rem * dt_rem));
                new_pos = f3_add(new_pos, f3_mul_f(entity.dp, dt_rem));
                new_pos = f3_add(new_pos, entity.pos);

                ray = f3_xy(f3_sub(new_pos, entity.pos));
                if (f2_mag2(ray) > 0.0) {
                    float m = f2_mag(ray) / dt_rem;
                    ray = f2_mul_f(f2_normalize(ray), m);
                }
            } else {
                entity.pos = f3_add(entity.pos, f3_mul_f(f2_xyz(ray), dt_rem));
                dt_rem -= dt_rem;
                assert(dt_rem <= 0.0);
            }

            F3 moved_to = entity.pos;
            F3 movement_ray = f3_sub(moved_to, moved_from);
        }

        entity.dp = f3_add(f3_mul_f(entity.ddp, SIM_DT), entity.dp);
        entity.pos = new_pos;

        entities[entity_i] = entity;
    }

    for (int i=0; i<arrlen(collisions); i++) {
        Collision collision = collisions[i];
        if ((collision.entity == player || collision.hit_entity == player) && collision.kind == COLLIDE_ENTITY) {
            health -= 1;
            // @TODO: Have some force system, don't just update positions or you'll get put in walls.
            float explode = 5.0;
            if (collision.hit_plane.x != 0.0) {
                if (entities[collision.entity].pos.x < entities[collision.hit_entity].pos.x) {
                    entities[collision.entity].dp.x -= explode;
                    entities[collision.hit_entity].dp.x += explode;
                } else {
                    entities[collision.entity].dp.x += explode;
                    entities[collision.hit_entity].dp.x -= explode;
                }
            }
            if (collision.hit_plane.y != 0.0) {
                if (entities[collision.entity].pos.y < entities[collision.hit_entity].pos.y) {
                    entities[collision.entity].dp.y -= explode;
                    entities[collision.hit_entity].dp.y += explode;
                } else {
                    entities[collision.entity].dp.y += explode;
                    entities[collision.hit_entity].dp.y -= explode;
                }
            }
        }
    }

    if (health <= 0) {
        dead = true;
    }

    sim_time += SIM_DT;
} 

// Renderer types
// Fixed angle camera for now.
typedef struct {
    F3 position;
    F3 target;
} RenderCamera;

typedef enum {
    COLOR_DEBUG_PINK,
    COLOR_LIGHT_GRAY,
    COLOR_WHITE,
    COLOR_GREEN,
    COLOR_BROWN,
    COLOR_DARK_BROWN,
    COLOR_DARK_GREEN,
    COLOR_PURPLE,
    COLOR_DARK_GRAY,
    COLOR_LIGHT_BLUE,
    COLOR_BLUE,
    COLOR_DARK_BLUE,
    COLOR_RED,
    NUM_RENDER_COLORS,
} RenderColor;

typedef struct {
    F3 center;
    F3 extent;
    RenderColor color;
} RenderCube;

const RenderColor tile_render_color[NUM_TILE_KINDS] = {
    [TILE_NONE] = COLOR_WHITE,
    [TILE_FLOOR] = COLOR_LIGHT_GRAY,
    [TILE_GRASS] = COLOR_GREEN,
    [TILE_DIRT] = COLOR_BROWN,
    [TILE_ROAD] = COLOR_DARK_BROWN,
    [TILE_BUSH] = COLOR_DARK_GREEN,
    [TILE_WALL] = COLOR_PURPLE,
    [TILE_ROCK] = COLOR_DARK_GRAY,
    [TILE_SHALLOW_WATER] = COLOR_LIGHT_BLUE,
    [TILE_DEEP_WATER] = COLOR_DARK_BLUE,
};

const float tile_height[NUM_TILE_KINDS] = {
    [TILE_NONE] = 0.0,
    [TILE_FLOOR] = 0.0,
    [TILE_GRASS] = 0.0,
    [TILE_DIRT] = 0.0,
    [TILE_ROAD] = 0.0,
    [TILE_BUSH] = 1.0,
    [TILE_WALL] = 2.0,
    [TILE_ROCK] = 1.0,
    [TILE_SHALLOW_WATER] = 0.0,
    [TILE_DEEP_WATER] = 0.0,
};

// Renderer state.
RenderCamera camera = {0};
RenderCube *render_cubes = NULL;

// Game State
double last_update_time = 0.0;
double dt_remaining = 0.0;

bool editing = false;

void game_update_and_render() {
    // Update with fixed timestep simulation.
    if (!editing && !dead) {
        double dt = dt_remaining + MIN(1.0, t - last_update_time);
        while (dt > SIM_DT) {
            dt -= SIM_DT;
            sim_tick();
            frame_ticks++;
        }
        dt_remaining = dt;
    }
    last_update_time = t;


    int camera_min_x = entities[player].pos.x - 12;
    int camera_max_x = entities[player].pos.x + 12;
    int camera_min_y = entities[player].pos.y - 8;
    int camera_max_y = entities[player].pos.y + 8;

    int z = 0;
    for (int y=MAX(camera_min_y, 0); y<MIN(camera_max_y, TILE_MAP_DEPTH); y++) {
        for (int x = MAX(camera_min_x, 0); x < MIN(camera_max_x, TILE_MAP_WIDTH); x++) {
            char tile_char = map_i(x, y);
            TileKind tile_kind = tile_from_char(tile_char);

            if (tile_kind == TILE_NONE) {
                continue;
            }

            RenderColor color = tile_render_color[tile_kind];
            float height = tile_height[tile_kind];

            // Maybe this is like a distinction between the tile and the object placed on it? I dunno yet.
            arrpush(render_cubes, (RenderCube){
                    .center = f3((float)x, (float)y, -0.5),
                    .extent = f3(0.5, 0.5, 0.5),
                    .color = color
            });

            if (height > 0.0) {
                arrpush(render_cubes, (RenderCube){
                        .center = f3((float)x, (float)y, height*0.5),
                        .extent = f3(0.5, 0.5, height*0.5),
                        .color = color
                });
            }
        }
    }

    // Render
    // TODO: Interpolate positions with dt_remaining.
    camera.target = entities[player].pos;
    camera.position = f3_add(camera.target, f3(0.0,-5.0,10.0));

    for (int i=0; i<arrlen(entities); i++) {
        Entity *entity = entities + i;
        switch(entity->kind) {
            case PLAYER: {
                arrpush(render_cubes, (RenderCube){
                        .center = f3_add(entity->pos, f3_mul_f(f3_z, 0.5)),
                        .extent = f3(0.25, 0.25, 0.5),
                        .color = COLOR_BLUE
                });
            } break;
            case BADGUY: {
                arrpush(render_cubes, (RenderCube){
                        .center = f3_add(entity->pos, f3_mul_f(f3_z, 0.5)),
                        .extent = f3(0.25, 0.25, 0.5),
                        .color = COLOR_RED
                });
            } break;
        }
    }
}

// Platform internal constants.
int screen_width = 1920/2;
int screen_height = 1080/2;

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"

Vector3 rl(F3 v) {
    return (Vector3){.x = v.x, .y = v.y, .z = v.z};
}

void platform_setup() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "Game");
    SetTextureFilter(GetFontDefault().texture, FILTER_POINT);
}

void platform_update() {
    t = GetTime();
    input.w.down = IsKeyDown(KEY_W);
    input.a.down = IsKeyDown(KEY_A);
    input.s.down = IsKeyDown(KEY_S);
    input.d.down = IsKeyDown(KEY_D);

    arrclear(render_cubes);
}

Color render_color(RenderColor color) {
    switch (color) {
    case COLOR_DEBUG_PINK:
        return PINK;
        break;
    case COLOR_WHITE:
        return RAYWHITE;
        break;
    case COLOR_LIGHT_GRAY:
        return LIGHTGRAY;
        break;
    case COLOR_GREEN:
        return GREEN;
        break;
    case COLOR_BROWN:
        return BROWN;
        break;
    case COLOR_DARK_BROWN:
        return DARKBROWN;
        break;
    case COLOR_DARK_GREEN:
        return DARKGREEN;
        break;
    case COLOR_PURPLE:
        return PURPLE;
        break;
    case COLOR_DARK_GRAY:
        return DARKGRAY;
        break;
    case COLOR_LIGHT_BLUE:
        return BLUE;
        break;
    case COLOR_BLUE:
        return BLUE;
        break;
    case COLOR_DARK_BLUE:
        return DARKBLUE;
        break;
    case COLOR_RED:
        return RED;
        break;
    }
    return PINK;
}

void platform_draw() {
    Camera3D raylib_camera = (Camera3D){
            .type = CAMERA_PERSPECTIVE,
            .position = rl(camera.position),
            .target = rl(camera.target),
            .up = (Vector3){.x=0.0, .y=0.0, .z=1.0},
            .fovy = 45.0
    };

    BeginDrawing();
    ClearBackground(RAYWHITE);

    Ray ray = {0};
    if (editing) {
        ray = GetMouseRay(GetMousePosition(), raylib_camera);
    }

    BeginMode3D(raylib_camera);
    for (int i=0; i<arrlen(render_cubes); i++) {
        Vector3 size = rl(f3_mul_f(render_cubes[i].extent,  2.0));
        Vector3 wire_size = rl(f3_mul_f(render_cubes[i].extent, 2.01));
        Vector3 pos = rl(render_cubes[i].center);

        Color color = render_color(render_cubes[i].color);
        DrawCubeV(pos, size, color);

        if (editing && CheckCollisionRayBox(ray,
                (BoundingBox){(Vector3){ .x=pos.x - size.x/2, .y=pos.y - size.y/2, .z=pos.z - size.z/2 },
                              (Vector3){ .x=pos.x + size.x/2, .y=pos.y + size.y/2, .z=pos.z + size.z/2 }})) {
            DrawCubeWiresV(pos, wire_size, PINK);
        } else {
            DrawCubeWiresV(pos, wire_size, BLACK);
        }
    }
    EndMode3D();

    int x = 10;
    for (int i=0; i<health; i++) {
        DrawCircle(x, 10, 5, RED);
        x+= 12;
    }

    DrawFPS(10, GetScreenHeight() - 25);
    
    //arrclear(text_buf);
    //arrprintf(text_buf, "Ticks this frame: %i", frame_ticks);
    //DrawText(text_buf, 5, 20, 24, BLACK);

    if (dead) {
        DrawText("YOU ARE DEAD", GetScreenWidth()/2, GetScreenHeight()/2, 50, RED);
    }

    if (IsKeyPressed(KEY_P)) {
        editing = !editing;
    }

    if (editing) {
        if (GuiButton((Rectangle){100, 100, 100, 100}, "#05#Fuck You")) { printf("fuck you\n"); }
    }

    EndDrawing();
}

int main() {
    game_setup();
    platform_setup();
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        reset_debug();
        platform_update();
        game_update_and_render();
        platform_draw();
    }
    CloseWindow();
    return 0;
}

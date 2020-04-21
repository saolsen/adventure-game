// TODO List
// Port sweep_aabb to the inline spot I set up.

// Start iterating on some real gameplay.
// Move steve.h stuff into this file too. (literate programming?????)

// Don't set up itch.io yet you fucker. Get further in the game first.

// @NOTE: This is c++20+ only
// Because I use designated initializers (c99) AND operator overloading (c++).
// It's really just c99 + operator overloading but that's only c++20 for some reason.

// TODO: Just inline whatever you do use. Easier to share the code.
#include "steve.h"
#include "steve.cpp"

// Debug State
// Because of the setup of this project I can just write to this anywhere and render it.
// Super useful
int frame_ticks = 0;

// Called every frame, reset stuff you want to reset every frame here.
void reset_debug() {
    frame_ticks = 0;
}

// Map generation
const size_t TILE_MAP_WIDTH = 18;
const size_t TILE_MAP_DEPTH = 18;
const size_t TILE_MAP_HEIGHT = 4;

const char TILE_MAP[TILE_MAP_HEIGHT][TILE_MAP_DEPTH * TILE_MAP_WIDTH] = {
        {
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
                'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
        },
        {
                'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', 'W', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'W', 'W', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W',
        },
        {
                'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', 'W', 'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
                'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W',
        },
        {
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        },
};

inline char map_i(size_t x, size_t y, size_t z) {
    return TILE_MAP[z][((TILE_MAP_DEPTH - 1 - y) * TILE_MAP_WIDTH) + x];
}

// Platform layer types
struct DigitalButton {
    bool down;
    bool pressed;
    bool released;
};

struct Input {
    DigitalButton w;
    DigitalButton a;
    DigitalButton s;
    DigitalButton d;
};

// Platform layer state
Input input = {};
double t = 0;

// Simulation types
struct SweepResult {
    bool hit;
    float hit_time;
    f2 hit_normal;
};

struct Aabb {
    f2 center;
    f2 extent;
};

enum EntityKind {
    PLAYER,
    BADGUY,
};

struct Entity {
    EntityKind kind;
    f3 pos;
    f3 dp;
    f3 ddp;

    f3 target;
};

// Simulation constants
const float SIM_DT = 1.0 / 60.0;
const float PLAYER_MAX_SPEED = 30.0;
const float PLAYER_ACCELERATION = 20.0;
const float BADGUY_MAX_SPEED = 25.0;
const float BADGUY_ACCELERATION = 15.0;
const float FRICTION = 4.0;

// Simulation state
double sim_time = 0.0;
Entity *entities = NULL;
size_t player = 0;

void game_setup() {
    arrpush(entities, (Entity){
            .kind = PLAYER,
    });
    arrpush(entities, (Entity){
            .kind = BADGUY,
            .pos = v3(5.0, 5.0, 0.0),
    });
}

void sim_tick() {
    f3 player_accel = {};
    if (input.w.down) { player_accel += f3_y; }
    if (input.a.down) { player_accel -= f3_x; }
    if (input.s.down) { player_accel -= f3_y; }
    if (input.d.down) { player_accel += f3_x; }

    player_accel = normalize(player_accel);

    for (int i=0; i<arrlen(entities); i++) {
        Entity entity = entities[i];
        float max_speed = 0.0;
        switch(entity.kind) {
            case PLAYER: {
                entity.ddp = player_accel;
                entity.ddp *= PLAYER_ACCELERATION;
                max_speed = PLAYER_MAX_SPEED;
            } break;
            case BADGUY: {
                entity.target = entities[player].pos;
                entity.ddp = normalize(entity.target - entity.pos);
                entity.ddp *= BADGUY_ACCELERATION;
                max_speed = BADGUY_MAX_SPEED;
            } break;
        };

        f3 new_dp = entity.ddp * SIM_DT + entity.dp;
        f3 friction = -new_dp * FRICTION;
        new_dp += friction * SIM_DT;

        if (mag2(new_dp) > max_speed * max_speed) {
            new_dp = normalize(new_dp) * max_speed;
        }

        entity.dp = new_dp;

        f3 new_pos = 0.5 * entity.ddp * (SIM_DT * SIM_DT) + entity.dp * SIM_DT + entity.pos;
        f2 ray = v2(new_pos - entity.pos);
        if (mag2(ray) > 0.0) {
            float m = mag(ray) / SIM_DT;
            ray = normalize(ray) * m;
        }

        Aabb entity_geometry = (Aabb){
                .center = v2(entity.pos),
                .extent = v2(0.25, 0.25),
        };

        float dt_rem = SIM_DT;
        while (dt_rem > 0.0) {
            float min_hit_t = HUGE_VAL;
            f2 hit_plane = f2_zero;

            // TODO: Iterate over tiles to see if we hit them.
            size_t tile_z = entity.pos.z + 1;
            for (int y=0; y<TILE_MAP_DEPTH; y++) {
                for (int x=0; x<TILE_MAP_WIDTH; x++) {
                    char tile = map_i(x, y, tile_z);
                    if (tile == ' ') { continue; }
                    Aabb tile_geometry = (Aabb){
                            .center = v2((float)x, (float)y),
                            .extent = v2(0.5, 0.5),
                    };

                    // Inline fn sweep_aabb
                    SweepResult result;
                    f2 *sweep_ray = &ray;
                    {
                        result = {};

                    }
                    if (result.hit && result.hit_time < min_hit_t) {
                        min_hit_t = result.hit_time;
                        hit_plane = result.hit_normal;
                    }
                }
            }

            f3 moved_from = entity.pos;

            if (min_hit_t < HUGE_VAL) {
                entity.pos += v3(ray) * (float)(min_hit_t - 0.0001);
                dt_rem -= min_hit_t;
            } else {
                entity.pos += v3(ray) * dt_rem;
                dt_rem -= dt_rem;
                assert(dt_rem <= 0.0);

                if (hit_plane.x != 0.0) {
                    entity.dp.x = 0.0;
                    entity.ddp.x = 0.0;
                }
                if (hit_plane.y != 0.0) {
                    entity.dp.y = 0.0;
                    entity.ddp.y = 0.0;
                }

                new_pos = 0.5 * entity.ddp * (dt_rem * dt_rem) + entity.dp * dt_rem + entity.pos;
                ray = v2(new_pos - entity.pos);
                if (mag2(ray) > 0.0) {
                    float m = mag(ray) / SIM_DT;
                    ray = normalize(ray) * m;
                }
            }

            f3 moved_to = entity.pos;
            f3 movement_ray = moved_to - moved_from;
        }

        entity.dp = entity.ddp * SIM_DT + entity.dp;
        entity.pos = new_pos;

        entities[i] = entity;
    }

    sim_time += SIM_DT;
}

// Renderer types
// Fixed angle camera for now.
struct RenderCamera {
    f3 position;
    f3 target;
};

enum RenderColor {
    COLOR_DEBUG_PINK,
    COLOR_BLACK,
    COLOR_WHITE,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_RED,
};

struct RenderCube {
    f3 center;
    f3 extent;
    RenderColor color;
};

// Renderer state.
RenderCamera camera = {};
RenderCube *render_cubes = NULL;

// Game State
double last_update_time = 0.0;
double dt_remaining = 0.0;

void game_update_and_render() {
    // Update with fixed timestep simulation.
    double dt = dt_remaining + MIN(1.0, t - last_update_time);
    while (dt > SIM_DT) {
        dt -= SIM_DT;
        sim_tick();
        frame_ticks++;
    }
    dt_remaining = dt;
    last_update_time = t;

    // TODO: Only render tiles on camera.
    for (int z=0; z<TILE_MAP_HEIGHT; z++) {
        for (int y=0; y<TILE_MAP_DEPTH; y++) {
            for (int x = 0; x < TILE_MAP_WIDTH; x++) {
                char tile = map_i(x, y, z);
                if (tile == ' ') {
                    continue;
                }

                RenderColor color;
                switch(tile) {
                    case 'x': {
                        color = COLOR_WHITE;
                    } break;
                    case 'W': {
                        color = COLOR_BLUE;
                    } break;
                    default: {
                        color = COLOR_DEBUG_PINK;
                    } break;
                };

                arrpush(render_cubes, (RenderCube){
                        .center = v3((float)x, (float)y, (float)z-0.5),
                        .extent = v3(0.5, 0.5, 0.5),
                        .color = color
                });
            }
        }
    }

    // Render
    // TODO: Interpolate positions with dt_remaining.
    camera.target = entities[player].pos;
    camera.position = camera.target + (f3){.x=0.0,.y=-5.0,.z=10.0};

    for (int i=0; i<arrlen(entities); i++) {
        Entity *entity = entities + i;
        switch(entity->kind) {
            case PLAYER: {
                arrpush(render_cubes, (RenderCube){
                        .center = entity->pos + (f3_z * 0.5),
                        .extent = v3(0.25, 0.25, 0.5),
                        .color = COLOR_BLUE
                });
            } break;
            case BADGUY: {
                arrpush(render_cubes, (RenderCube){
                        .center = entity->pos + (f3_z * 0.5),
                        .extent = v3(0.25, 0.25, 0.5),
                        .color = COLOR_RED
                });
            } break;
        };
    }
}

// Platform internal constants.
int screen_width = 1920/2;
int screen_height = 1080/2;

#include "raylib.h"

Vector3 rl(f3 v) {
    return (Vector3){.x = v.x, .y = v.y, .z = v.z};
}

void raylib_setup() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "Game");
    SetTextureFilter(GetFontDefault().texture, FILTER_POINT);
}

void raylib_update() {
    t = GetTime();
    input.w.down = IsKeyDown(KEY_W);
    input.a.down = IsKeyDown(KEY_A);
    input.s.down = IsKeyDown(KEY_S);
    input.d.down = IsKeyDown(KEY_D);

    arrclear(render_cubes);
}

void raylib_draw() {
    Camera3D raylib_camera = (Camera3D){
            .type = CAMERA_PERSPECTIVE,
            .position = rl(camera.position),
            .target = rl(camera.target),
            .up = Vector3{.x=0.0, .y=0.0, .z=1.0},
            .fovy = 45.0
    };

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(raylib_camera);
    for (int i=0; i<arrlen(render_cubes); i++) {
        Vector3 size = rl(render_cubes[i].extent * 2.0);
        Vector3 wire_size = rl(render_cubes[i].extent * 2.01);
        Vector3 pos = rl(render_cubes[i].center);

        Color color;
        switch(render_cubes[i].color) {
            case COLOR_DEBUG_PINK: {
                color = PINK;
            } break;
            case COLOR_WHITE: {
                color = RAYWHITE;
            } break;
            case COLOR_BLACK: {
                color = BLACK;
            } break;
            case COLOR_GREEN: {
                color = GREEN;
            } break;
            case COLOR_BLUE: {
                color = BLUE;
            } break;
            case COLOR_RED: {
                color = RED;
            } break;
            default: {
                color = PINK;
            } break;
        };
        DrawCubeV(pos, size, color);
        DrawCubeWiresV(pos, wire_size, BLACK);
    }
    EndMode3D();

    DrawFPS(10, 10);
    char *txt = NULL;
    arrprintf(txt, "Ticks this frame: %i", frame_ticks);
    DrawText(txt, 10, 40, 20, DARKGRAY);
    arrfree(txt);
    EndDrawing();
}

int main() {
    game_setup();
    raylib_setup();
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        reset_debug();
        raylib_update();
        game_update_and_render();
        raylib_draw();
    }
    CloseWindow();
    return 0;
}
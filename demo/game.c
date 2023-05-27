
#include "body.h"
#include "collision.h"
#include "forces.h"
#include "list.h"
#include "map.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "star.h"
#include "state.h"
#include "tank.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

const double MAX_WIDTH_GAME = 1600.0;
const double MAX_HEIGHT_GAME = 1200.0;

// tank types
size_t WALL_TYPE = 0;
size_t BULLET_TYPE = 1;
size_t DEFAULT_TANK_TYPE = 2;

// default tank stats
double DEFAULT_TANK_VELOCITY = 100.0;
double DEFAULT_TANK_STARTING_HEALTH = 100.0;
double DEFAULT_TANK_SIDE_LENGTH = 80.0;
double DEFAULT_TANK_MASS = 100.0;

rgb_color_t PLAYER1_COLOR = {1.0, 0.0, 0.0};
rgb_color_t PLAYER2_COLOR = {0.0, 1.0, 0.0};

typedef struct state {
  scene_t *scene;
  double time;
} state_t;

void handler1(char key, key_event_type_t type, double held_time,
             state_t *state) {
  body_t *player1 = scene_get_body(state->scene, (size_t)0);

  if (type == KEY_PRESSED) {
    switch (key) {
      case UP_ARROW: {
        vector_t up_vel = {
            cos(body_get_rotation(player1)) * DEFAULT_TANK_VELOCITY,
            sin(body_get_rotation(player1)) * DEFAULT_TANK_VELOCITY};
        body_set_velocity(player1, up_vel);
        break;
      }
      case DOWN_ARROW: {
        vector_t down_vel = {
            cos(body_get_rotation(player1)) * -DEFAULT_TANK_VELOCITY,
            sin(body_get_rotation(player1)) * -DEFAULT_TANK_VELOCITY};
        body_set_velocity(player1, down_vel);
        break;
      }
      case RIGHT_ARROW: {
        double curr_vel_x =
            body_get_velocity(player1).x / cos(body_get_rotation(player1));
        double curr_vel_y =
            body_get_velocity(player1).y / sin(body_get_rotation(player1));
        body_set_rotation(player1, body_get_rotation(player1) - M_PI / 45);
        if (body_get_velocity(player1).x != 0 &&
            body_get_velocity(player1).y != 0) {
          vector_t vel = {cos(body_get_rotation(player1)) * curr_vel_x,
                          sin(body_get_rotation(player1)) * curr_vel_y};
          body_set_velocity(player1, vel);
        }
        break;
      }
      case LEFT_ARROW: {
        double curr_vel_x =
            body_get_velocity(player1).x / cos(body_get_rotation(player1));
        double curr_vel_y =
            body_get_velocity(player1).y / sin(body_get_rotation(player1));
        body_set_rotation(player1, body_get_rotation(player1) + M_PI / 45);
        if (body_get_velocity(player1).x != 0 &&
            body_get_velocity(player1).y != 0) {
          vector_t vel = {cos(body_get_rotation(player1)) * curr_vel_x,
                          sin(body_get_rotation(player1)) * curr_vel_y};
          body_set_velocity(player1, vel);
        }
        break;
      }
    }
  }
  if (type == KEY_RELEASED) {
    switch (key) {
      case UP_ARROW: {
        body_set_velocity(player1, VEC_ZERO);
        break;
      }
      case DOWN_ARROW: {
        body_set_velocity(player1, VEC_ZERO);
        break;
      }
    }
  }
}

void handler2(char key, key_event_type_t type, double held_time,
             state_t *state) {
  body_t *player2 = scene_get_body(state->scene, (size_t)1);

  if (type == KEY_PRESSED) {
    switch (key) {
      case 'w': {
        vector_t up_vel = {
            cos(body_get_rotation(player2)) * DEFAULT_TANK_VELOCITY,
            sin(body_get_rotation(player2)) * DEFAULT_TANK_VELOCITY};
        body_set_velocity(player2, up_vel);
        break;
      }
      case 's': {
        vector_t down_vel = {
            cos(body_get_rotation(player2)) * -DEFAULT_TANK_VELOCITY,
            sin(body_get_rotation(player2)) * -DEFAULT_TANK_VELOCITY};
        body_set_velocity(player2, down_vel);
        break;
      }
      case 'd': {
        double curr_vel_x =
            body_get_velocity(player2).x / cos(body_get_rotation(player2));
        double curr_vel_y =
            body_get_velocity(player2).y / sin(body_get_rotation(player2));
        body_set_rotation(player2, body_get_rotation(player2) - M_PI / 45);
        if (body_get_velocity(player2).x != 0 &&
            body_get_velocity(player2).y != 0) {
          vector_t vel = {cos(body_get_rotation(player2)) * curr_vel_x,
                          sin(body_get_rotation(player2)) * curr_vel_y};
          body_set_velocity(player2, vel);
        }
        break;
      }
      case 'a': {
        double curr_vel_x =
            body_get_velocity(player2).x / cos(body_get_rotation(player2));
        double curr_vel_y =
            body_get_velocity(player2).y / sin(body_get_rotation(player2));
        body_set_rotation(player2, body_get_rotation(player2) + M_PI / 45);
        if (body_get_velocity(player2).x != 0 &&
            body_get_velocity(player2).y != 0) {
          vector_t vel = {cos(body_get_rotation(player2)) * curr_vel_x,
                          sin(body_get_rotation(player2)) * curr_vel_y};
          body_set_velocity(player2, vel);
        }
        break;
      }
    }
  }
  if (type == KEY_RELEASED) {
    switch (key) {
      case 'w': {
        body_set_velocity(player2, VEC_ZERO);
        break;
      }
      case 's': {
        body_set_velocity(player2, VEC_ZERO);
        break;
      }
    }
  }
}

void handler(char key, key_event_type_t type, double held_time, state_t *state) {
  handler1(key, type, held_time, state);
  handler2(key, type, held_time, state);
}

state_t *emscripten_init() {
  vector_t min = VEC_ZERO;
  vector_t max = {MAX_WIDTH_GAME, MAX_HEIGHT_GAME};
  sdl_init(min, max);
  state_t *state = malloc(sizeof(state_t));
  assert(state != NULL);
  state->scene = scene_init();

  vector_t player1_start =
      (vector_t){MAX_WIDTH_GAME / 6, MAX_HEIGHT_GAME - 300.0};
  vector_t player2_start =
      (vector_t){MAX_WIDTH_GAME * 5 / 6, MAX_HEIGHT_GAME / 2};
  // can channge it to choose the type of tank later
  tank_t *player1 = init_default_tank(
      player1_start, DEFAULT_TANK_SIDE_LENGTH, VEC_ZERO, DEFAULT_TANK_MASS,
      PLAYER1_COLOR, DEFAULT_TANK_STARTING_HEALTH, DEFAULT_TANK_TYPE);
  tank_t *player2 = init_default_tank(
      player2_start, DEFAULT_TANK_SIDE_LENGTH, VEC_ZERO, DEFAULT_TANK_MASS,
      PLAYER2_COLOR, DEFAULT_TANK_STARTING_HEALTH, DEFAULT_TANK_TYPE);
  body_set_rotation(tank_get_body(player2), M_PI);
  scene_add_body(state->scene, tank_get_body(player1));
  scene_add_body(state->scene, tank_get_body(player2));
  map_init(state->scene);

  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  sdl_on_key((key_handler_t)handler);
  state->time += dt;

  scene_tick(state->scene, dt);
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}

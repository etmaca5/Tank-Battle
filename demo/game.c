
#include "body.h"
#include "collision.h"
#include "forces.h"
#include "list.h"
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
size_t DEFAULT_TANK_TYPE =2; 

// default tank stats
double DEFAULT_TANK_VELOCITY = 100.0;
double DEFAULT_TANK_STARTING_HEALTH = 100.0;
double DEFAULT_TANK_SIDE_LENGTH = 80.0;
double DEFAULT_TANK_MASS = 100.0;

rgb_color_t PLAYER1_COLOR = {0.5,0.5,0.5};
rgb_color_t PLAYER2_COLOR = {0.0,1.0,0.0};



typedef struct state {
  scene_t *scene;
  double time;
} state_t;




void handler(char key, key_event_type_t type, double held_time,
             state_t *state) {
//   body_t *player1 = scene_get_body(state->scene, (size_t)0);
//   body_t *player2 = scene_get_body(state->scene, (size_t)0);

//   if (type == KEY_PRESSED) {
//     switch (key) {
//     case RIGHT_ARROW: {
//       break;
//     }
//     case LEFT_ARROW: {
//       break;
//     }
//     }
//   }
//   if (type == KEY_RELEASED) {
//     switch (key) {
//     case RIGHT_ARROW: {
//       break;
//     }
//     case LEFT_ARROW: {
//       break;
//     }
//     }
//   }
}


state_t *emscripten_init() {
  vector_t min = VEC_ZERO;
  vector_t max = {MAX_WIDTH_GAME, MAX_HEIGHT_GAME};
  sdl_init(min, max);
  state_t *state = malloc(sizeof(state_t));
  assert(state != NULL);
  state->scene = scene_init();

  vector_t player1_start = (vector_t){MAX_WIDTH_GAME / 6 , MAX_HEIGHT_GAME / 2};
  vector_t player2_start = (vector_t){MAX_WIDTH_GAME * 5 / 6 , MAX_HEIGHT_GAME / 2};
  //can channge it to choose the type of tank later
  tank_t *player1 = init_default_tank(player1_start, DEFAULT_TANK_SIDE_LENGTH, VEC_ZERO,  DEFAULT_TANK_MASS, PLAYER1_COLOR, DEFAULT_TANK_STARTING_HEALTH, DEFAULT_TANK_TYPE);
  tank_t *player2 = init_default_tank(player2_start, DEFAULT_TANK_SIDE_LENGTH, VEC_ZERO,  DEFAULT_TANK_MASS, PLAYER2_COLOR, DEFAULT_TANK_STARTING_HEALTH, DEFAULT_TANK_TYPE);
  scene_add_body(state->scene, tank_get_body(player1));
  scene_add_body(state->scene, tank_get_body(player2));
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




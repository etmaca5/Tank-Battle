
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
const double MAX_HEIGHT_GAME = 1300.0;

// default tank stats
double DEFAULT_TANK_VELOCITY = 100.0;
double DEFAULT_TANK_STARTING_HEALTH = 100.0;
double DEFAULT_TANK_SIDE_LENGTH = 80.0;
double DEFAULT_TANK_MASS = 100.0;
double DEFAULT_TANK_ROTATION_SPEED = M_PI / 2;
double DEFAULT_TANK_MAX_HEALTH = 100.0;

double BULLET_HEIGHT = 25.0;
double BULLET_WIDTH = 10.0;
double BULLET_MASS = 5.0;
double BULLET_VELOCITY = 250.0;
double RELOAD_SPEED = 1.0;
double BULLET_DISAPPEAR_TIME = 10.0;

double HEALTH_BAR_WIDTH = 500.0;
double HEALTH_BAR_HEIGHT = 100.0;

rgb_color_t PLAYER1_COLOR = {1.0, 0.0, 0.0};
rgb_color_t PLAYER2_COLOR = {0.0, 1.0, 0.0};

typedef struct state {
  scene_t *scene;
  double time;
} state_t;

list_t *make_health_bar_p1(double health) {
  list_t *shape = list_init(4, (free_func_t)free);

  vector_t *point1 = malloc(sizeof(vector_t));
  assert(point1 != NULL);
  point1->x = health / DEFAULT_TANK_MAX_HEALTH * HEALTH_BAR_WIDTH;
  point1->y = MAX_HEIGHT_GAME - HEALTH_BAR_HEIGHT;
  list_add(shape, point1);

  vector_t *point2 = malloc(sizeof(vector_t));
  assert(point2 != NULL);
  point2->x = health / DEFAULT_TANK_MAX_HEALTH * HEALTH_BAR_WIDTH;
  point2->y = MAX_HEIGHT_GAME;
  list_add(shape, point2);

  vector_t *point3 = malloc(sizeof(vector_t));
  assert(point3 != NULL);
  point3->x = 0.0;
  point3->y = MAX_HEIGHT_GAME;
  list_add(shape, point3);
  vector_t *point4 = malloc(sizeof(vector_t));
  assert(point4 != NULL);
  point4->x = 0.0;
  point4->y = MAX_HEIGHT_GAME - HEALTH_BAR_HEIGHT;
  list_add(shape, point4);
  return shape;
}

list_t *make_health_bar_p2(double health) {
  list_t *shape = list_init(4, (free_func_t)free);

  vector_t *point1 = malloc(sizeof(vector_t));
  assert(point1 != NULL);
  point1->x = MAX_WIDTH_GAME - health / DEFAULT_TANK_MAX_HEALTH * HEALTH_BAR_WIDTH;
  point1->y = MAX_HEIGHT_GAME;
  list_add(shape, point1);

  vector_t *point2 = malloc(sizeof(vector_t));
  assert(point2 != NULL);
  point2->x = MAX_WIDTH_GAME - health / DEFAULT_TANK_MAX_HEALTH * HEALTH_BAR_WIDTH;
  point2->y = MAX_HEIGHT_GAME - HEALTH_BAR_HEIGHT;
  list_add(shape, point2);

  vector_t *point3 = malloc(sizeof(vector_t));
  assert(point3 != NULL);
  point3->x = MAX_WIDTH_GAME;
  point3->y = MAX_HEIGHT_GAME - HEALTH_BAR_HEIGHT;
  list_add(shape, point3);

  vector_t *point4 = malloc(sizeof(vector_t));
  assert(point4 != NULL);
  point4->x = MAX_WIDTH_GAME;
  point4->y = MAX_HEIGHT_GAME;
  list_add(shape, point4);
  return shape;
}

list_t *make_bullet(vector_t edge) {
  list_t *shape = list_init(4, (free_func_t)free);
  vector_t *point1 = malloc(sizeof(vector_t));
  assert(point1 != NULL);
  point1->x = edge.x;
  point1->y = edge.y - BULLET_WIDTH / 2;
  list_add(shape, point1);

  vector_t *point2 = malloc(sizeof(vector_t));
  assert(point2 != NULL);
  point2->x = edge.x + BULLET_HEIGHT;
  point2->y = edge.y - BULLET_WIDTH / 2;
  list_add(shape, point2);

  vector_t *point3 = malloc(sizeof(vector_t));
  assert(point3 != NULL);
  point3->x = edge.x + BULLET_HEIGHT;
  point3->y = edge.y + BULLET_WIDTH / 2;
  list_add(shape, point3);

  vector_t *point4 = malloc(sizeof(vector_t));
  assert(point4 != NULL);
  point4->x = edge.x;
  point4->y = edge.y + BULLET_WIDTH / 2;
  list_add(shape, point4);
  return shape;
}

void handler1(char key, key_event_type_t type, double held_time,
              state_t *state) {
  body_t *player1 = scene_get_body(state->scene, (size_t)0);

  if (type == KEY_PRESSED) {
    switch (key) {
      case UP_ARROW: {
        body_set_magnitude(player1, DEFAULT_TANK_VELOCITY);
        break;
      }
      case DOWN_ARROW: {
        body_set_magnitude(player1, -DEFAULT_TANK_VELOCITY);
        break;
      }
      case RIGHT_ARROW: {
        body_set_rotation_speed(player1, -DEFAULT_TANK_ROTATION_SPEED);
        break;
      }
      case LEFT_ARROW: {
        body_set_rotation_speed(player1, DEFAULT_TANK_ROTATION_SPEED);
        break;
      }
      case SPACE: {
        if (body_get_time(player1) > RELOAD_SPEED) {
          body_set_time(player1, 0.0);
          vector_t spawn_point = body_get_centroid(player1);
          list_t *bullet_points = make_bullet(spawn_point);
          polygon_rotate(bullet_points, body_get_rotation(player1),
                        body_get_centroid(player1));
          vector_t player_dir = {cos(body_get_rotation(player1)),
                                sin(body_get_rotation(player1))};
          vector_t move_up =
              vec_multiply(DEFAULT_TANK_SIDE_LENGTH / 2 + 10, player_dir);
          polygon_translate(bullet_points, move_up);
          size_t *type = malloc(sizeof(size_t));
          *type = BULLET_TYPE;
          body_t *bullet = body_init_with_info(
              bullet_points, BULLET_MASS, PLAYER1_COLOR, type, (free_func_t)free);
          body_set_rotation_empty(bullet, body_get_rotation(player1));
          body_set_velocity(bullet, vec_multiply(BULLET_VELOCITY, player_dir));
          body_set_time(bullet, 0.0);
          scene_add_body(state->scene, bullet);

          // create collision with tanks
          create_partial_destructive_collision(
              state->scene, scene_get_body(state->scene, 0), bullet);
          create_partial_destructive_collision(
              state->scene, scene_get_body(state->scene, 1), bullet);

          // create collision with walls and other bullets
          for (size_t i = 2; i < scene_bodies(state->scene) - 1; i++) {
            body_t *body = scene_get_body(state->scene, i);
            if (*(size_t *)body_get_info(body) == BULLET_TYPE) {
              create_destructive_collision(state->scene, body, bullet);
            } else if (*(size_t *)body_get_info(body) == RECTANGLE_OBSTACLE_TYPE ||
                      *(size_t *)body_get_info(body) == TRIANGLE_OBSTACLE_TYPE) {
              create_physics_collision(state->scene, 1.0, bullet, body);
            }
          }
        }
      }
    }
  }
  if (type == KEY_RELEASED) {
    switch (key) {
    case UP_ARROW: {
      body_set_velocity(player1, VEC_ZERO);
      body_set_magnitude(player1, 0.0);
      break;
    }
    case DOWN_ARROW: {
      body_set_velocity(player1, VEC_ZERO);
      body_set_magnitude(player1, 0.0);
      break;
    }
    case RIGHT_ARROW: {
      body_set_rotation_speed(player1, 0.0);
      break;
    }
    case LEFT_ARROW: {
      body_set_rotation_speed(player1, 0.0);
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
        body_set_magnitude(player2, DEFAULT_TANK_VELOCITY);
        break;
      }
      case 's': {
        body_set_magnitude(player2, -DEFAULT_TANK_VELOCITY);
        break;
      }
      case 'd': {
        body_set_rotation_speed(player2, -DEFAULT_TANK_ROTATION_SPEED);
        break;
      }
      case 'a': {
        body_set_rotation_speed(player2, DEFAULT_TANK_ROTATION_SPEED);
        break;
      }
      case 'r': {
        if (body_get_time(player2) > RELOAD_SPEED) {
          body_set_time(player2, 0.0);
          vector_t spawn_point = body_get_centroid(player2);
          list_t *bullet_points = make_bullet(spawn_point);
          polygon_rotate(bullet_points, body_get_rotation(player2),
                        body_get_centroid(player2));
          vector_t player_dir = {cos(body_get_rotation(player2)),
                                sin(body_get_rotation(player2))};
          vector_t move_up =
              vec_multiply(DEFAULT_TANK_SIDE_LENGTH / 2 + 10, player_dir);
          polygon_translate(bullet_points, move_up);
          size_t *type = malloc(sizeof(size_t));
          *type = BULLET_TYPE;
          body_t *bullet = body_init_with_info(
              bullet_points, BULLET_MASS, PLAYER2_COLOR, type, (free_func_t)free);
          body_set_rotation_empty(bullet, body_get_rotation(player2));
          body_set_velocity(bullet, vec_multiply(BULLET_VELOCITY, player_dir));
          body_set_time(bullet, 0.0);
          scene_add_body(state->scene, bullet);

          // create collision with tanks
          create_partial_destructive_collision(
              state->scene, scene_get_body(state->scene, 0), bullet);
          create_partial_destructive_collision(
              state->scene, scene_get_body(state->scene, 1), bullet);

          // create collision with walls and other bullets
          for (size_t i = 2; i < scene_bodies(state->scene) - 1; i++) {
            body_t *body = scene_get_body(state->scene, i);
            if (*(size_t *)body_get_info(body) == BULLET_TYPE) {
              create_destructive_collision(state->scene, body, bullet);
            } else if (*(size_t *)body_get_info(body) == RECTANGLE_OBSTACLE_TYPE ||
                      *(size_t *)body_get_info(body) == TRIANGLE_OBSTACLE_TYPE) {
              create_physics_collision(state->scene, 1.0, bullet, body);
            }
          }
        }
      }
    }
  }
  if (type == KEY_RELEASED) {
    switch (key) {
    case 'w': {
      body_set_velocity(player2, VEC_ZERO);
      body_set_magnitude(player2, 0.0);
      break;
    }
    case 's': {
      body_set_velocity(player2, VEC_ZERO);
      body_set_magnitude(player2, 0.0);
      break;
    }
    case 'a': {
      body_set_rotation_speed(player2, 0.0);
      break;
    }
    case 'd': {
      body_set_rotation_speed(player2, 0.0);
      break;
    }
    }
  }
}

void handler(char key, key_event_type_t type, double held_time,
             state_t *state, vector_t loc) {
  handler1(key, type, held_time, state);
  handler2(key, type, held_time, state);
  switch(key) {
    case MOUSE_CLICK:
      printf("click\n");
      break;
    case MOUSE_MOVED:
      printf("moving\n");
      break;
  }
}

state_t *emscripten_init() {
  vector_t min = VEC_ZERO;
  vector_t max = {MAX_WIDTH_GAME, MAX_HEIGHT_GAME};
  sdl_init(min, max);
  state_t *state = malloc(sizeof(state_t));
  assert(state != NULL);
  state->scene = scene_init();

  vector_t player1_start =
      (vector_t){MAX_WIDTH_GAME / 6, MAX_HEIGHT_GAME - 400.0};
  vector_t player2_start =
      (vector_t){MAX_WIDTH_GAME * 5 / 6, MAX_HEIGHT_GAME / 2 - 50.0};
  // can channge it to choose the type of tank later
  tank_t *player1 = init_default_tank(
      player1_start, DEFAULT_TANK_SIDE_LENGTH, VEC_ZERO, DEFAULT_TANK_MASS,
      PLAYER1_COLOR, DEFAULT_TANK_STARTING_HEALTH, DEFAULT_TANK_TYPE);
  tank_t *player2 = init_default_tank(
      player2_start, DEFAULT_TANK_SIDE_LENGTH, VEC_ZERO, DEFAULT_TANK_MASS,
      PLAYER2_COLOR, DEFAULT_TANK_STARTING_HEALTH, DEFAULT_TANK_TYPE);
  body_set_rotation(tank_get_body(player2), M_PI);
  body_set_health(tank_get_body(player1), DEFAULT_TANK_MAX_HEALTH);
  body_set_health(tank_get_body(player2), DEFAULT_TANK_MAX_HEALTH);
  scene_add_body(state->scene, tank_get_body(player1));
  scene_add_body(state->scene, tank_get_body(player2));

  // initialize health bars
  list_t *p1_health_bar_shape = make_health_bar_p1(DEFAULT_TANK_MAX_HEALTH);
  size_t *type = malloc(sizeof(size_t));
  *type = HEALTH_BAR_TYPE;
  body_t *p1_health_bar = body_init_with_info(p1_health_bar_shape, 10.0, PLAYER1_COLOR, type, (free_func_t)free);
  scene_add_body(state->scene, p1_health_bar);

  list_t *p2_health_bar_shape = make_health_bar_p2(DEFAULT_TANK_MAX_HEALTH);
  size_t *type2 = malloc(sizeof(size_t));
  *type2 = HEALTH_BAR_TYPE;
  body_t *p2_health_bar = body_init_with_info(p2_health_bar_shape, 10.0, PLAYER2_COLOR, type2, (free_func_t)free);
  scene_add_body(state->scene, p2_health_bar);

  map_init(state->scene);

  for (size_t i = 2; i < scene_bodies(state->scene); i++) {
    create_physics_collision(state->scene, 10.0,
                             scene_get_body(state->scene, 0),
                             scene_get_body(state->scene, i));
    create_physics_collision(state->scene, 10.0,
                             scene_get_body(state->scene, 1),
                             scene_get_body(state->scene, i));
  }

  

  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  sdl_on_key((key_handler_t)handler);
  state->time += dt;

  // add time to player bodies for reload
  body_t *player1 = scene_get_body(state->scene, 0);
  body_t *player2 = scene_get_body(state->scene, 1);
  body_set_time(player1, body_get_time(player1) + dt);
  body_set_time(player2, body_get_time(player2) + dt);

  // add time to bullet bodies to see if they should disappear
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (*(size_t *)body_get_info(body) == BULLET_TYPE) {
      body_set_time(body, body_get_time(body) + dt);
      if (body_get_time(body) > BULLET_DISAPPEAR_TIME) {
        body_remove(body);
      }
    }
  }

  // update health bar
  body_t *health_bar_p1 = scene_get_body(state->scene, 2);
  body_set_shape(health_bar_p1, make_health_bar_p1(body_get_health(player1)));

  body_t *health_bar_p2 = scene_get_body(state->scene, 3);
  body_set_shape(health_bar_p2, make_health_bar_p2(body_get_health(player2)));

  scene_tick(state->scene, dt);
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}

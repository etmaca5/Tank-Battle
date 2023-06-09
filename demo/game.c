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
#include "text.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// types of different bodies
const size_t WALL_TYPE = 0;
const size_t BULLET_TYPE = 1;
const size_t SNIPER_BULLET_TYPE = 10;
const size_t DEFAULT_TANK_TYPE = 2;
const size_t MELEE_TANK_TYPE = 3;
const size_t SNIPER_TANK_TYPE = 4;
const size_t GRAVITY_TANK_TYPE = 5;
const size_t HEALTH_BAR_TYPE = 6;

int FONT_SIZE = 50;
int TITLE_SIZE = 100;
double CIRCLE_POINTS = 300.0;

const double MAX_WIDTH_GAME = 1600.0;
const double MAX_HEIGHT_GAME = 1300.0;

// elasticity between tank
double TANKS_ELASTICITY = 3.0;

// default tank stats
double DEFAULT_TANK_VELOCITY = 100.0;
double DEFAULT_TANK_SIDE_LENGTH = 80.0;
double DEFAULT_TANK_MASS = 100.0;
double DEFAULT_TANK_ROTATION_SPEED = M_PI / 2;
double DEFAULT_TANK_MAX_HEALTH = 50.0;


// MELEE tank stats
size_t MELEE_TANK_POINTS = 6;
double MELEE_TANK_VELOCITY = 180.0;
double MELEE_TANK_SIDE_LENGTH = 60.0;
double MELEE_TANK_MASS = 100.0;
double MELEE_TANK_ROTATION_SPEED = M_PI * 3 / 4;
double MELEE_TANK_MAX_HEALTH = 50.0;


// // SNIPER tank stats
// size_t MELEE_TANK_POINTS = 6;
// double MELEE_TANK_VELOCITY = 180.0;
// double MELEE_TANK_SIDE_LENGTH = 60.0;
// double MELEE_TANK_MASS = 100.0;
// double MELEE_TANK_ROTATION_SPEED = M_PI * 3 / 4;
// double MELEE_TANK_MAX_HEALTH = 50.0;
// const double MELEE_TANK_DAMAGE = 25.0;
double SNIPER_RELOAD_SPEED = 2.5;


//damage stored here
const double BULLET_DAMAGE = 10.0;
const double MELEE_TANK_DAMAGE = 25.0;
const double SNIPER_BULLET_DAMAGE = 25.0;

double BULLET_HEIGHT = 25.0;
double BULLET_WIDTH = 10.0;
double BULLET_MASS = 5.0;
double BULLET_VELOCITY = 300.0;
double RELOAD_SPEED = 1.0;
double BULLET_DISAPPEAR_TIME = 10.0;

double HEALTH_BAR_WIDTH = 500.0;
double HEALTH_BAR_HEIGHT = 50.0;
double HEALTH_BAR_OFFSET_HORIZONTAL = 50.0;
double HEALTH_BAR_OFFSET_VERTICAL = 25.0;

double COLLISION_ELASTICITY = 20.0;

// menu stats
double BUTTON_X_MIN = 404.0;
double BUTTON_X_MAX = 598.0;
double START_BUTTON_Y_MIN = 194.0;
double START_BUTTON_Y_MAX = 262.0;
double OPTIONS_BUTTON_Y_MIN = 289.0;
double OPTIONS_BUTTON_Y_MAX = 359.0;

double GAMMA = 1.0;

COLORS: 
rgb_color_t PLAYER1_COLOR = {1.0, 0.0, 0.0};
rgb_color_t PLAYER1_COLOR_SIMILAR = {0.5, 0.0, 0.0};
rgb_color_t PLAYER2_COLOR = {0.0, 1.0, 0.0};
rgb_color_t PLAYER2_COLOR_SIMILAR = {0.0, 0.5, 0.0};
rgb_color_t LIGHT_GREY = {0.86, 0.86, 0.86};
rgb_color_t GREEN = {0.0, 1.0, 0.0};
SDL_Color SDL_WHITE = {255, 255, 255, 255};
rgb_color_t SLATE_GREY = {0.72, 0.79, 0.89};
SDL_Color SDL_BLACK = {0, 0, 0, 255};
SDL_Color FOREST_GREEN = {74, 103, 65, 255};

typedef struct state {
  scene_t *scene;
  double time;
  int player1_score;
  size_t player1_tank_type;
  size_t player2_tank_type;
  int player2_score;
  bool singleplayer;
  bool is_menu;
  text_t *text;
  text_t *title;
  text_t *scoreboard;
} state_t;

list_t *make_half_circle(vector_t center, double radius) {
  list_t *shape = list_init(18, (free_func_t)free);
  for (size_t i = 0; i < 18; i++) {
    vector_t *point = malloc(sizeof(vector_t));
    assert(point != NULL);
    point->x = center.x + radius;
    point->y = center.y;
    list_add(shape, point);
    polygon_rotate(shape, M_PI / 18, center);
  }
  vector_t *point = malloc(sizeof(vector_t));
  assert(point != NULL);
  point->x = center.x + radius;
  point->y = center.y;
  list_add(shape, point);
  return shape;
}

list_t *make_heart(vector_t center, double length) {
  list_t *shape = list_init(100, (free_func_t)free);

  // create first half circle
  vector_t rotation_area1 = {center.x + length / 2, center.y};
  list_t *half_circle1 = make_half_circle(rotation_area1, length / 2);
  // have to use int here since size_t is unsigned
  for (int i = (int)list_size(half_circle1) - 1; i >= 0; i--) {
    vector_t *point = list_get(half_circle1, i);
    list_add(shape, point);
  }

  // create second half circle
  vector_t rotation_area2 = {center.x - length / 2, center.y};
  list_t *half_circle2 = make_half_circle(rotation_area2, length / 2);
  for (int i = (int)list_size(half_circle2) - 1; i >= 0; i--) {
    vector_t *point = list_get(half_circle2, i);
    list_add(shape, point);
  }

  vector_t *bottom_point = malloc(sizeof(vector_t));
  *bottom_point = (vector_t){center.x, center.y - length};
  list_add(shape, bottom_point);
  return shape;
}

list_t *make_health_bar_p1(double health) {
  list_t *shape = list_init(4, (free_func_t)free);

  vector_t *point1 = malloc(sizeof(vector_t));
  assert(point1 != NULL);
  point1->x = health / DEFAULT_TANK_MAX_HEALTH * HEALTH_BAR_WIDTH +
              HEALTH_BAR_OFFSET_HORIZONTAL;
  point1->y = MAX_HEIGHT_GAME - HEALTH_BAR_HEIGHT - HEALTH_BAR_OFFSET_VERTICAL;
  list_add(shape, point1);

  vector_t *point2 = malloc(sizeof(vector_t));
  assert(point2 != NULL);
  point2->x = health / DEFAULT_TANK_MAX_HEALTH * HEALTH_BAR_WIDTH +
              HEALTH_BAR_OFFSET_HORIZONTAL;
  point2->y = MAX_HEIGHT_GAME - HEALTH_BAR_OFFSET_VERTICAL;
  list_add(shape, point2);

  vector_t *point3 = malloc(sizeof(vector_t));
  assert(point3 != NULL);
  point3->x = +HEALTH_BAR_OFFSET_HORIZONTAL;
  point3->y = MAX_HEIGHT_GAME - HEALTH_BAR_OFFSET_VERTICAL;
  list_add(shape, point3);
  vector_t *point4 = malloc(sizeof(vector_t));
  assert(point4 != NULL);
  point4->x = +HEALTH_BAR_OFFSET_HORIZONTAL;
  point4->y = MAX_HEIGHT_GAME - HEALTH_BAR_HEIGHT - HEALTH_BAR_OFFSET_VERTICAL;
  list_add(shape, point4);
  return shape;
}

list_t *make_health_bar_p2(double health) {
  list_t *shape = list_init(4, (free_func_t)free);

  vector_t *point1 = malloc(sizeof(vector_t));
  assert(point1 != NULL);
  point1->x = MAX_WIDTH_GAME -
              health / DEFAULT_TANK_MAX_HEALTH * HEALTH_BAR_WIDTH -
              HEALTH_BAR_OFFSET_HORIZONTAL;
  point1->y = MAX_HEIGHT_GAME - HEALTH_BAR_OFFSET_VERTICAL;
  list_add(shape, point1);

  vector_t *point2 = malloc(sizeof(vector_t));
  assert(point2 != NULL);
  point2->x = MAX_WIDTH_GAME -
              health / DEFAULT_TANK_MAX_HEALTH * HEALTH_BAR_WIDTH -
              HEALTH_BAR_OFFSET_HORIZONTAL;
  point2->y = MAX_HEIGHT_GAME - HEALTH_BAR_HEIGHT - HEALTH_BAR_OFFSET_VERTICAL;
  list_add(shape, point2);

  vector_t *point3 = malloc(sizeof(vector_t));
  assert(point3 != NULL);
  point3->x = MAX_WIDTH_GAME - HEALTH_BAR_OFFSET_HORIZONTAL;
  point3->y = MAX_HEIGHT_GAME - HEALTH_BAR_HEIGHT - HEALTH_BAR_OFFSET_VERTICAL;
  list_add(shape, point3);

  vector_t *point4 = malloc(sizeof(vector_t));
  assert(point4 != NULL);
  point4->x = MAX_WIDTH_GAME - HEALTH_BAR_OFFSET_HORIZONTAL;
  point4->y = MAX_HEIGHT_GAME - HEALTH_BAR_OFFSET_VERTICAL;
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

void handle_bullet(state_t *state, body_t *player, rgb_color_t color) {
  body_set_time(player, 0.0);
  vector_t spawn_point = body_get_centroid(player);
  list_t *bullet_points = make_bullet(spawn_point);
  polygon_rotate(bullet_points, body_get_rotation(player),
                 body_get_centroid(player));
  vector_t player_dir = {cos(body_get_rotation(player)),
                         sin(body_get_rotation(player))};
  vector_t move_up =
      vec_multiply(DEFAULT_TANK_SIDE_LENGTH / 2 + 10, player_dir);
  polygon_translate(bullet_points, move_up);
  size_t *type = malloc(sizeof(size_t));
  *type = BULLET_TYPE;
  body_t *bullet = body_init_with_info(bullet_points, BULLET_MASS, color, type,
                                       (free_func_t)free);
  body_set_rotation_empty(bullet, body_get_rotation(player));
  body_set_velocity(bullet, vec_multiply(BULLET_VELOCITY, player_dir));
  body_set_time(bullet, 0.0);
  scene_add_body(state->scene, bullet);

  // create collision with tanks
  create_partial_destructive_collision(state->scene,
                                       scene_get_body(state->scene, 0), bullet);
  create_partial_destructive_collision(state->scene,
                                       scene_get_body(state->scene, 1), bullet);

  // add drag force
  create_drag(state->scene, GAMMA, bullet);

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

void tank_handler(char key, key_event_type_t type, double held_time,
                  state_t *state, body_t *player, rgb_color_t player_color) {
  if (*(size_t *)body_get_info(player) ==
      DEFAULT_TANK_TYPE) { // this is to handle the different tank types
    if (type == KEY_PRESSED) {
      switch (key) {
      case UP_ARROW: {
        body_set_magnitude(player, DEFAULT_TANK_VELOCITY);
        break;
      }
      case DOWN_ARROW: {
        body_set_magnitude(player, -DEFAULT_TANK_VELOCITY);
        break;
      }
      case RIGHT_ARROW: {
        body_set_rotation_speed(player, -DEFAULT_TANK_ROTATION_SPEED);
        break;
      }
      case LEFT_ARROW: {
        body_set_rotation_speed(player, DEFAULT_TANK_ROTATION_SPEED);
        break;
      }
      case SPACE: {
        if (body_get_time(player) > RELOAD_SPEED) {
          handle_bullet(state, player, player_color);
        }
      }
      }
    }
  } else if (*(size_t *)body_get_info(player) ==
             MELEE_TANK_TYPE) { // handles MELEE tank
    if (type == KEY_PRESSED) {
      switch (key) {
      case UP_ARROW: {
        body_set_magnitude(player, MELEE_TANK_VELOCITY);
        break;
      }
      case DOWN_ARROW: {
        body_set_magnitude(player, -MELEE_TANK_VELOCITY);
        break;
      }
      case RIGHT_ARROW: {
        body_set_rotation_speed(player, -MELEE_TANK_ROTATION_SPEED);
        break;
      }
      case LEFT_ARROW: {
        body_set_rotation_speed(player, MELEE_TANK_ROTATION_SPEED);
        break;
      }
      }
    }
  } else {
    // add other tanks after
  }
  if (type == KEY_RELEASED) {
    switch (key) {
    case UP_ARROW: {
      body_set_velocity(player, VEC_ZERO);
      body_set_magnitude(player, 0.0);
      break;
    }
    case DOWN_ARROW: {
      body_set_velocity(player, VEC_ZERO);
      body_set_magnitude(player, 0.0);
      break;
    }
    case RIGHT_ARROW: {
      body_set_rotation_speed(player, 0.0);
      break;
    }
    case LEFT_ARROW: {
      body_set_rotation_speed(player, 0.0);
      break;
    }
    }
  }
}

void tank_handler2(char key, key_event_type_t type, double held_time,
                   state_t *state, body_t *player, rgb_color_t player_color) {
  if (*(size_t *)body_get_info(player) ==
      DEFAULT_TANK_TYPE) { // this is to handle the different tank types
    if (type == KEY_PRESSED) {
      switch (key) {
      case 'w': {
        body_set_magnitude(player, DEFAULT_TANK_VELOCITY);
        break;
      }
      case 's': {
        body_set_magnitude(player, -DEFAULT_TANK_VELOCITY);
        break;
      }
      case 'd': {
        body_set_rotation_speed(player, -DEFAULT_TANK_ROTATION_SPEED);
        break;
      }
      case 'a': {
        body_set_rotation_speed(player, DEFAULT_TANK_ROTATION_SPEED);
        break;
      }
      case 'r': {
        if (body_get_time(player) > RELOAD_SPEED) {
          handle_bullet(state, player, player_color);
        }
      }
      }
    }
  } else if (*(size_t *)body_get_info(player) ==
             MELEE_TANK_TYPE) { // handles MELEE tank
    if (type == KEY_PRESSED) {
      switch (key) {
      case 'w': {
        body_set_magnitude(player, MELEE_TANK_VELOCITY);
        break;
      }
      case 's': {
        body_set_magnitude(player, -MELEE_TANK_VELOCITY);
        break;
      }
      case 'd': {
        body_set_rotation_speed(player, -MELEE_TANK_ROTATION_SPEED);
        break;
      }
      case 'a': {
        body_set_rotation_speed(player, MELEE_TANK_ROTATION_SPEED);
        break;
      }
      }
    }
  } else {
    // add other tanks after
  }
  if (type == KEY_RELEASED) {
    switch (key) {
    case 'w': {
      body_set_velocity(player, VEC_ZERO);
      body_set_magnitude(player, 0.0);
      break;
    }
    case 's': {
      body_set_velocity(player, VEC_ZERO);
      body_set_magnitude(player, 0.0);
      break;
    }
    case 'd': {
      body_set_rotation_speed(player, 0.0);
      break;
    }
    case 'a': {
      body_set_rotation_speed(player, 0.0);
      break;
    }
    }
  }
}

double double_abs(double x) {
  if (x < 0)
    return -x;
  return x;
}

void reset_mode(body_t *ai) {
  body_set_ai_mode(ai, 0);
  body_set_ai_time(ai, 0.0);
}

void ai_aim(body_t *player, body_t *ai) {
  // program ai to aim towards enemy, works for default tank
  if (body_get_distance(body_get_centroid(ai), body_get_centroid(player)) <
      750.0) {
    vector_t distance =
        vec_subtract(body_get_centroid(player), body_get_centroid(ai));
    double angle = atan(distance.y / distance.x);
    if (distance.x < 0) {
      angle += M_PI;
    }
    angle =
        angle -
        2 * M_PI * ((size_t)angle / ((size_t)(2 * M_PI))); // simulate % by 2pi
    double ai_angle = body_get_rotation(ai);
    ai_angle =
        ai_angle -
        2 * M_PI * ((size_t)angle / ((size_t)(2 * M_PI))); // simulate % by 2pi
    if (ai_angle < angle) {
      body_set_rotation_speed(ai, DEFAULT_TANK_ROTATION_SPEED);
    } else {
      body_set_rotation_speed(ai, -DEFAULT_TANK_ROTATION_SPEED);
    }
  } else {
    body_set_rotation_speed(ai, 0.0);
  }
}

void ai_shoot(state_t *state, body_t *player, body_t *ai) {
  if (body_get_distance(body_get_centroid(ai), body_get_centroid(player)) <
      750.0) {
    vector_t distance =
        vec_subtract(body_get_centroid(player), body_get_centroid(ai));
    double angle = atan(distance.y / distance.x);
    if (distance.x < 0) {
      angle += M_PI;
    }
    angle =
        angle -
        2 * M_PI * ((size_t)angle / ((size_t)(2 * M_PI))); // simulate % by 2pi
    double ai_angle = body_get_rotation(ai);
    ai_angle =
        ai_angle -
        2 * M_PI * ((size_t)angle / ((size_t)(2 * M_PI))); // simulate % by 2pi

    // program ai to shoot randomly, but only if pointed somewhat close to enemy
    // tank
    if (double_abs(angle - ai_angle) < M_PI / 8) {
      double time = body_get_time(ai);
      if (time > rand_num(RELOAD_SPEED, RELOAD_SPEED * 3)) {
        handle_bullet(state, ai, PLAYER2_COLOR);
      }
    }
  }
}

void move_ai(state_t *state, double dt) {
  body_t *player = scene_get_body(state->scene, 0);
  body_t *ai = scene_get_body(state->scene, 1);
  size_t ai_mode = body_get_ai_mode(ai);
  double ai_time = body_get_ai_time(ai);
  ai_shoot(state, player, ai);

  if (ai_mode == 0) {
    ai_aim(player, ai);

    body_set_velocity(ai, VEC_ZERO);
    body_set_magnitude(ai, 0.0);
    bool move = (ai_time > rand_num(2.5, 5.0));
    if (move) {
      size_t rand_mode = (size_t)rand_num(0.0, 10.0);
      body_set_ai_mode(ai, rand_mode);
      body_set_ai_time(ai, 0.0);
    }
  } else if (ai_mode == 1) {
    if (body_get_just_collided(ai)) {
      body_set_ai_mode(ai, 2);
      body_set_ai_time(ai, 1.5 - ai_time);
      body_set_just_collided(ai, false);
    } else {
      body_set_magnitude(ai, DEFAULT_TANK_VELOCITY);
      if (ai_time > 1.5) {
        reset_mode(ai);
      }
    }
  } else if (ai_mode == 2) {
    if (body_get_just_collided(ai)) {
      body_set_ai_mode(ai, 1);
      body_set_ai_time(ai, 1.5 - ai_time);
      body_set_just_collided(ai, false);
    } else {
      body_set_magnitude(ai, -DEFAULT_TANK_VELOCITY);
      if (ai_time > 1.5) {
        reset_mode(ai);
      }
    }
  } else if (ai_mode == 3) {
    if (body_get_just_collided(ai)) {
      body_set_ai_mode(ai, 6);
      body_set_ai_time(ai, 1.5 - ai_time);
      body_set_just_collided(ai, false);
    } else {
      body_set_magnitude(ai, DEFAULT_TANK_VELOCITY);
      body_set_rotation_speed(ai, DEFAULT_TANK_ROTATION_SPEED);
      if (ai_time > 1.5) {
        reset_mode(ai);
      }
    }
  } else if (ai_mode == 4) {
    if (body_get_just_collided(ai)) {
      body_set_ai_mode(ai, 5);
      body_set_ai_time(ai, 1.5 - ai_time);
      body_set_just_collided(ai, false);
    } else {
      body_set_magnitude(ai, DEFAULT_TANK_VELOCITY);
      body_set_rotation_speed(ai, -DEFAULT_TANK_ROTATION_SPEED);
      if (ai_time > 1.5) {
        reset_mode(ai);
      }
    }
  } else if (ai_mode == 5) {
    if (body_get_just_collided(ai)) {
      body_set_ai_mode(ai, 4);
      body_set_ai_time(ai, 1.5 - ai_time);
      body_set_just_collided(ai, false);
    } else {
      body_set_magnitude(ai, -DEFAULT_TANK_VELOCITY);
      body_set_rotation_speed(ai, DEFAULT_TANK_ROTATION_SPEED);
      if (ai_time > 1.5) {
        reset_mode(ai);
      }
    }
  } else if (ai_mode == 6) {
    if (body_get_just_collided(ai)) {
      body_set_ai_mode(ai, 3);
      body_set_ai_time(ai, 1.5 - ai_time);
      body_set_just_collided(ai, false);
    } else {
      body_set_magnitude(ai, -DEFAULT_TANK_VELOCITY);
      body_set_rotation_speed(ai, -DEFAULT_TANK_ROTATION_SPEED);
      if (ai_time > 1.5) {
        reset_mode(ai);
      }
    }
  } else if (ai_mode == 7) {
    body_set_rotation_speed(ai, DEFAULT_TANK_ROTATION_SPEED);
    if (ai_time > 1) {
      reset_mode(ai);
    }
  } else if (ai_mode == 8) {
    body_set_rotation_speed(ai, DEFAULT_TANK_ROTATION_SPEED);
    if (ai_time > 0.5) {
      reset_mode(ai);
    }
  } else if (ai_mode == 9) {
    body_set_rotation_speed(ai, -DEFAULT_TANK_ROTATION_SPEED);
    if (ai_time > 0.5) {
      reset_mode(ai);
    }
  }
}

void check_end_game(state_t *state) {
  if (state->player1_score == 3 || state->player2_score == 3) {
    exit(0);
  }
}

void show_scoreboard(state_t *state, int player1_score, int player2_score) {
  vector_t corner = {600.0, MAX_HEIGHT_GAME - 25.0};
  list_t *points = make_rectangle(corner, 400.0, 150.0);
  rgb_color_t black = {0.0, 0.0, 0.0};
  sdl_draw_polygon(points, black);

  SDL_Color white = {255, 255, 255, 255};
  // loc
  vector_t score_loc = {675.0, MAX_HEIGHT_GAME - 13.0};

  char player1_str[20];
  sprintf(player1_str, "%d", player1_score);
  char player2_str[20];
  sprintf(player2_str, "%d", player2_score);
  char *final_str = (char *)malloc(
      sizeof(char) * (strlen(player1_str) + strlen(player2_str) + 2));
  strcpy(final_str, player1_str);
  strcat(final_str, "   -   ");
  strcat(final_str, player2_str);

  TTF_Font *font1 = TTF_OpenFont("assets/font.ttf", FONT_SIZE);
  text_t *text = text_init(font1, (free_func_t)free);
  state->scoreboard = text;
  SDL_Texture *scoreboard =
      sdl_load_text(state, final_str, state->text, white, score_loc);

  sdl_show();
  SDL_DestroyTexture(scoreboard);
}
body_t *handle_selected_tank(size_t tank_type, vector_t start_pos,
                             rgb_color_t color) {
  // add rest of the tanks
  if (tank_type == DEFAULT_TANK_TYPE) {
    return init_default_tank(start_pos, DEFAULT_TANK_SIDE_LENGTH, VEC_ZERO,
                             DEFAULT_TANK_MASS, color, DEFAULT_TANK_MAX_HEALTH,
                             DEFAULT_TANK_TYPE);
  } else if (tank_type == MELEE_TANK_TYPE) {
    return init_melee_tank(start_pos, MELEE_TANK_SIDE_LENGTH, MELEE_TANK_POINTS,
                           VEC_ZERO, MELEE_TANK_MASS, color,
                           MELEE_TANK_MAX_HEALTH, MELEE_TANK_TYPE);
  } else {
    return init_default_tank(start_pos, DEFAULT_TANK_SIDE_LENGTH, VEC_ZERO,
                             DEFAULT_TANK_MASS, color, DEFAULT_TANK_MAX_HEALTH,
                             DEFAULT_TANK_TYPE);
  }
}

void make_players(state_t *state) {
  vector_t player1_start =
      (vector_t){MAX_WIDTH_GAME / 6, MAX_HEIGHT_GAME - 400.0};
  vector_t player2_start =
      (vector_t){MAX_WIDTH_GAME * 5 / 6, MAX_HEIGHT_GAME / 2 - 50.0};
  // can channge it to choose the type of tank later
  body_t *player1 = handle_selected_tank(state->player1_tank_type,
                                         player1_start, PLAYER1_COLOR);
  body_t *player2 = handle_selected_tank(state->player2_tank_type,
                                         player2_start, PLAYER2_COLOR);
  body_set_rotation(player2, M_PI);
  body_set_health(player1, DEFAULT_TANK_MAX_HEALTH);
  body_set_health(player2, DEFAULT_TANK_MAX_HEALTH);
  scene_add_body(state->scene, player1);
  scene_add_body(state->scene, player2);
}

void make_health_bars(state_t *state) {
  // initialize health bars
  list_t *p1_health_bar_shape = make_health_bar_p1(DEFAULT_TANK_MAX_HEALTH);
  size_t *type = malloc(sizeof(size_t));
  *type = HEALTH_BAR_TYPE;
  body_t *p1_health_bar = body_init_with_info(
      p1_health_bar_shape, 10.0, PLAYER1_COLOR, type, (free_func_t)free);
  scene_add_body(state->scene, p1_health_bar);

  list_t *p2_health_bar_shape = make_health_bar_p2(DEFAULT_TANK_MAX_HEALTH);
  size_t *type2 = malloc(sizeof(size_t));
  *type2 = HEALTH_BAR_TYPE;
  body_t *p2_health_bar = body_init_with_info(
      p2_health_bar_shape, 10.0, PLAYER2_COLOR, type2, (free_func_t)free);
  scene_add_body(state->scene, p2_health_bar);

  vector_t P1_HEART_CENTER = {50.0, MAX_HEIGHT_GAME - 40.0};
  vector_t P2_HEART_CENTER = {MAX_WIDTH_GAME - 50.0, MAX_HEIGHT_GAME - 40.0};

  list_t *p1_heart = make_heart(P1_HEART_CENTER, 50.0);
  size_t *type3 = malloc(sizeof(size_t));
  *type3 = HEALTH_BAR_TYPE;
  body_t *p1_heart_body = body_init_with_info(
      p1_heart, 10.0, PLAYER1_COLOR_SIMILAR, type3, (free_func_t)free);
  scene_add_body(state->scene, p1_heart_body);

  list_t *p2_heart = make_heart(P2_HEART_CENTER, 50.0);
  size_t *type4 = malloc(sizeof(size_t));
  *type4 = HEALTH_BAR_TYPE;
  body_t *p2_heart_body = body_init_with_info(
      p2_heart, 10.0, PLAYER2_COLOR_SIMILAR, type4, (free_func_t)free);
  scene_add_body(state->scene, p2_heart_body);
}

void reset_game(state_t *state) {
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    body_remove(body);
  }
  scene_tick(state->scene, 0.0);

  make_players(state);
  make_health_bars(state);
  map_init(state->scene);
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (*(size_t *)body_get_info(body) == RECTANGLE_OBSTACLE_TYPE ||
        *(size_t *)body_get_info(body) == TRIANGLE_OBSTACLE_TYPE) {
      create_physics_collision(state->scene, COLLISION_ELASTICITY,
                               scene_get_body(state->scene, 0),
                               scene_get_body(state->scene, i));
      create_physics_collision(state->scene, COLLISION_ELASTICITY,
                               scene_get_body(state->scene, 1),
                               scene_get_body(state->scene, i));
    }
  }
}

void check_game_end(state_t *state) {
  body_t *player1 = scene_get_body(state->scene, 0);
  body_t *player2 = scene_get_body(state->scene, 1);
  if (body_get_health(player1) <= 0) {
    state->player2_score++;
    reset_game(state);
  } else if (body_get_health(player2) <= 0) {
    state->player1_score++;
    reset_game(state);
  }
}

bool start_button_pressed(vector_t mouse) {
  if (mouse.x >= BUTTON_X_MIN && mouse.x <= BUTTON_X_MAX &&
      mouse.y >= START_BUTTON_Y_MIN && mouse.y <= START_BUTTON_Y_MAX) {
    return true;
  }
  return false;
}

bool options_button_pressed(vector_t mouse) {
  if (mouse.x >= BUTTON_X_MIN && mouse.x <= BUTTON_X_MAX &&
      mouse.y >= OPTIONS_BUTTON_Y_MIN && mouse.y <= OPTIONS_BUTTON_Y_MAX) {
    return true;
  }
  return false;
}

void menu_init(state_t *state) {
  state->is_menu = true;

  TTF_Font *font1 = TTF_OpenFont("assets/font.ttf", FONT_SIZE);
  text_t *text = text_init(font1, (free_func_t)free);
  state->text = text;

  TTF_Font *font2 = TTF_OpenFont("assets/font.ttf", TITLE_SIZE);
  text_t *title = text_init(font2, (free_func_t)free);
  state->title = title;
}

void menu_pop_up(state_t *state) {
  vector_t corner1 = {0.0, MAX_HEIGHT_GAME};
  list_t *background = make_rectangle(corner1, MAX_WIDTH_GAME, MAX_HEIGHT_GAME);
  sdl_draw_polygon(background, LIGHT_GREY);

  // start button
  vector_t corner2 = {550.0, 750.0};
  list_t *start_button = make_rectangle(corner2, 500.0, 180.0);
  sdl_draw_polygon(start_button, GREEN);

  vector_t start_loc = {680.0, 750.0};
  SDL_Texture *start =
      sdl_load_text(state, "Start!", state->text, SDL_WHITE, start_loc);

  // options button
  vector_t corner3 = {550.0, 500.0};
  list_t *options_button = make_rectangle(corner3, 500.0, 180.0);
  sdl_draw_polygon(options_button, SLATE_GREY);

  // options text
  vector_t options_loc = {640.0, 500.0};
  SDL_Texture *options =
      sdl_load_text(state, "Options", state->text, SDL_BLACK, options_loc);

  // title
  vector_t title_loc = {540.0, 1120.0};
  SDL_Texture *title =
      sdl_load_text(state, "Tanks", state->title, FOREST_GREEN, title_loc);

  sdl_show();
  SDL_DestroyTexture(start);
  SDL_DestroyTexture(options);
  SDL_DestroyTexture(title);
}

void handler(char key, key_event_type_t type, double held_time, state_t *state,
             vector_t loc) {
  if (state->is_menu) {
    switch (key) {
    case MOUSE_CLICK: {
      if (start_button_pressed(loc)) {
        state->is_menu = false;
        break;
      } else if (options_button_pressed(loc)) {
        // show tank options
      }
    }
    }
  } else {
    body_t *player1 = scene_get_body(state->scene, (size_t)0);
    body_t *player2 = scene_get_body(state->scene, (size_t)1);
    tank_handler(key, type, held_time, state, player1, PLAYER1_COLOR);
    if (!state->singleplayer) {
      tank_handler2(key, type, held_time, state, player2, PLAYER2_COLOR);
    }
  }
}

state_t *emscripten_init() {
  vector_t min = VEC_ZERO;
  vector_t max = {MAX_WIDTH_GAME, MAX_HEIGHT_GAME};
  sdl_init(min, max);
  state_t *state = malloc(sizeof(state_t));
  assert(state != NULL);
  state->time = 0.0;
  state->scene = scene_init();
  state->player1_score = 0;
  state->player2_score = 0;
  state->player1_tank_type = DEFAULT_TANK_TYPE;
  state->player2_tank_type = MELEE_TANK_TYPE;
  state->singleplayer = false;

  menu_init(state); // will have to add menu feature that allows selection of
                    // the tank type (from global vars)

  make_players(state);

  make_health_bars(state);

  map_init(state->scene);

  show_scoreboard(state, 0, 0);

  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (*(size_t *)body_get_info(body) == RECTANGLE_OBSTACLE_TYPE ||
        *(size_t *)body_get_info(body) == TRIANGLE_OBSTACLE_TYPE) {
      create_physics_collision(state->scene, COLLISION_ELASTICITY,
                               scene_get_body(state->scene, 0),
                               scene_get_body(state->scene, i));
      create_physics_collision(state->scene, COLLISION_ELASTICITY,
                               scene_get_body(state->scene, 1),
                               scene_get_body(state->scene, i));
    }
  }
  return state;
}

void emscripten_main(state_t *state) {
  sdl_clear();
  if (state->is_menu) {
    menu_pop_up(state);
    sdl_on_key((key_handler_t)handler);
  } else {
    double dt = time_since_last_tick();
    sdl_on_key((key_handler_t)handler);
    state->time += dt;
    check_game_end(state);

    // add time to player bodies for reload
    body_t *player1 = scene_get_body(state->scene, 0);
    body_t *player2 = scene_get_body(state->scene, 1);
    body_set_time(player1, body_get_time(player1) + dt);
    body_set_time(player2, body_get_time(player2) + dt);

    if (state->singleplayer) {
      move_ai(state, dt);
      body_set_ai_time(player2, body_get_ai_time(player2) + dt);
    }

    // add time to bullet bodies to see if they should disappear
    for (size_t i = 0; i < scene_bodies(state->scene); i++) {
      body_t *body = scene_get_body(state->scene, i);
      if (*(size_t *)body_get_info(body) == BULLET_TYPE ||
          *(size_t *)body_get_info(body) == SNIPER_BULLET_TYPE) {
        body_set_time(body, body_get_time(body) + dt);
        if (body_get_time(body) > BULLET_DISAPPEAR_TIME) {
          body_remove(body);
        }
      }
    }

    // //update health bar
    body_t *health_bar_p1 = scene_get_body(state->scene, 2);
    body_set_shape(health_bar_p1, make_health_bar_p1(body_get_health(player1)));

    body_t *health_bar_p2 = scene_get_body(state->scene, 3);
    body_set_shape(health_bar_p2, make_health_bar_p2(body_get_health(player2)));

    scene_tick(state->scene, dt);
    sdl_render_scene(state->scene);
    show_scoreboard(state, state->player1_score, state->player2_score);
    check_end_game(state);
  }
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}
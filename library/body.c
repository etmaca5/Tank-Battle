
#include "color.h"
#include "list.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "vector.h"
#include <assert.h>
#include <collision.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct body {
  double mass;
  list_t *shape;
  vector_t velocity;
  vector_t centroid;
  double rotation;
  rgb_color_t color;
  vector_t force;
  vector_t impulse;
  void *info;
  free_func_t freer;
  bool is_removed;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  body_t *body = malloc(sizeof(body_t));
  assert(body != NULL);
  body->shape = shape;
  body->velocity = VEC_ZERO;
  body->centroid = polygon_centroid(shape);
  body->color = color;
  body->rotation = 0.0;
  body->mass = mass;
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
  body->info = NULL;
  body->freer = (free_func_t)free;
  body->is_removed = false;
  return body;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer) {
  body_t *body = body_init(shape, mass, color);
  if (mass == INFINITY) {
    body->velocity = VEC_ZERO;
  }
  body->info = info;
  body->freer = info_freer;
  if (info_freer == NULL) {
    body->freer = (free_func_t)free;
  }
  return body;
}

void body_free(body_t *body) {
  list_free(body->shape);
  // body->freer(body->info);
  free(body);
}

list_t *body_get_shape(body_t *body) {
  list_t *lst = list_init(list_size(body->shape), &free);
  for (size_t i = 0; i < list_size(body->shape); i++) {
    vector_t *elem = list_get(body->shape, i);
    vector_t *elem_copy = malloc(sizeof(vector_t));
    elem_copy->x = elem->x;
    elem_copy->y = elem->y;
    list_add(lst, elem_copy);
  }
  return lst;
}

vector_t body_get_centroid(body_t *body) { return body->centroid; }

vector_t body_get_velocity(body_t *body) { return body->velocity; }

vector_t body_get_force(body_t *body) { return body->force; }

vector_t body_get_impulse(body_t *body) { return body->impulse; }

void *body_get_info(body_t *body) { return body->info; }

rgb_color_t body_get_color(body_t *body) { return body->color; }

void body_set_centroid(body_t *body, vector_t x) {
  polygon_translate(body->shape, vec_subtract(x, body->centroid));
  body->centroid = x;
}

void body_set_force(body_t *body, vector_t v) { body->force = v; }

void body_set_impulse(body_t *body, vector_t v) { body->impulse = v; }

void body_set_velocity(body_t *body, vector_t v) { body->velocity = v; }

void body_set_rotation(body_t *body, double angle) {
  polygon_rotate(body->shape, -1 * body->rotation, body->centroid);
  polygon_rotate(body->shape, angle, body->centroid);
  body->rotation = angle;
}

void body_combine_mass(body_t *body1, body_t *body2) {
  body1->mass = body1->mass + body2->mass;
}

void body_tick(body_t *body, double dt) {
  // get acceleration
  vector_t old_velocity = body_get_velocity(body);
  vector_t acceleration =
      vec_multiply(1.0 / body_get_mass(body), body_get_force(body));
  vector_t changed_velocity =
      vec_add(body->velocity, vec_multiply(dt, acceleration));
  body->velocity = changed_velocity;
  // get velocity
  vector_t impulse =
      vec_multiply(1.0 / body_get_mass(body), body_get_impulse(body));
  vector_t changed_velocity2 = vec_add(body->velocity, impulse);
  body->velocity = changed_velocity2;

  // average velocity
  vector_t added = vec_add(old_velocity, body->velocity);
  vector_t average = vec_multiply(0.5, added);

  // vector_t translation = {dt * (body->velocity.x), dt * (body->velocity.y)};
  vector_t translation = {dt * (average.x), dt * (average.y)};
  polygon_translate(body->shape, translation);
  body->centroid = polygon_centroid(body->shape);

  // resets impulse and force
  body_set_force(body, VEC_ZERO);
  body_set_impulse(body, VEC_ZERO);
}

void body_add_force(body_t *body, vector_t force) {
  body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulse = vec_add(body->impulse, impulse);
}

double body_get_distance(vector_t body1_centroid, vector_t body2_centroid) {
  return (double)sqrt(pow(body1_centroid.x - body2_centroid.x, 2) +
                      pow(body1_centroid.y - body2_centroid.y, 2));
}

double body_get_mass(body_t *body) { return body->mass; }

void body_remove(body_t *body) {
  if (body->is_removed == false) {
    body->is_removed = true;
  }
}

bool body_is_removed(body_t *body) { return body->is_removed; }

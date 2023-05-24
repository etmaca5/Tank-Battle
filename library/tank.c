#include "tank.h"
#include "color.h"
#include "list.h"
#include "body.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

typedef struct tank {
  body_t *body;
  size_t type;
  vector_t *velocity;
  double length;
  double rotation;
  double health;
} tank_t;

/*
will have function to intialize different kinds of tanks,
can start with a default tank with average stats all round
*/

/*
default tank
*/
tank_t *init_default_tank(vector_t center, double side_length,
                          vector_t velocity, double mass, rgb_color_t color,
                          double max_health, size_t tank_type) {
  tank_t *tank = malloc(sizeof(tank_t));
  assert(tank != NULL);

  list_t *tank_points = list_init(4, (free_func_t)free);
  // creates the points for the tank
  vector_t *point1 = malloc(sizeof(vector_t));
  assert(point1 != NULL);
  point1->x = center.x + side_length / 2;
  point1->y = center.y + side_length / 2;
  ;
  list_add(tank_points, point1);
  vector_t *point2 = malloc(sizeof(vector_t));
  assert(point2 != NULL);
  point2->x = center.x - side_length / 2;
  point2->y = center.y + side_length / 2;
  ;
  list_add(tank_points, point2);
  vector_t *point3 = malloc(sizeof(vector_t));
  assert(point3 != NULL);
  point3->x = center.x - side_length / 2;
  point3->y = center.y - side_length / 2;
  ;
  list_add(tank_points, point3);
  vector_t *point4 = malloc(sizeof(vector_t));
  assert(point4 != NULL);
  point4->x = center.x + side_length / 2;
  point4->y = center.y - side_length / 2;
  ;
  list_add(tank_points, point4);

  size_t *type = malloc(sizeof(size_t));
  *type = tank_type;
  tank->body = body_init_with_info(tank_points, mass, color, type,
                                    (free_func_t)body_free);
  tank->health = max_health;
  tank->rotation = 0.0;
  tank->length = side_length;

  vector_t *vel = malloc(sizeof(vector_t));
  assert(vel != NULL);
  *vel = VEC_ZERO;
  tank->velocity = vel;
  tank->type = tank_type;
  return tank;
}

body_t *tank_get_body(tank_t *tank){
    return tank->body;
}

list_t *tank_get_shape(tank_t *tank){
    return body_get_shape(tank->body);
}

void tank_free(tank_t *tank){
    //to be implemented
}

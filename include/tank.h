#ifndef __TANK_H__
#define __TANK_H__

#include "body.h"
#include "sdl_wrapper.h"

typedef struct tank tank_t;

tank_t *init_default_tank(vector_t center, double side_length,
                          vector_t velocity, double mass, rgb_color_t color,
                          double max_health, size_t tank_type);

body_t *tank_get_body(tank_t *tank);

list_t *tank_get_shape(tank_t *tank);

void tank_free(tank_t *tank);

void tank_set_velocity(tank_t *tank, vector_t vel);

vector_t tank_get_velocity(tank_t *tank);

void tank_damage_health(tank_t *tank, double damage);

double tank_get_health(tank_t *tank);

#endif // #ifndef __STAR_H__
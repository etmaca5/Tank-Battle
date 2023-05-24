
#include "body.h"
#include "collision.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "tank.h"
#include "sdl_wrapper.h"
#include "star.h"
#include "state.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>




//tank types

size_t WALL_TYPE  = 0;
size_t BULLET_TYPE  = 1;
size_t DEFAULT_TANK_TYPE  = 2; // only tank type defined for now, will increase later, this can be adjusted to account for the walls, bullets, etc.



//default tank stats
double DEFAUKLT_TANK_VELOCITY = 100.0;
double DEFAUKT_TANK_STARTING_HEALTH = 100.0;

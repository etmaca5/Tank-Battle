#ifndef __map_H__
#define __map_H__

#include "body.h"
#include "sdl_wrapper.h"
#include "scene.h"

extern const size_t RECTANGLE_OBSTACLE_TYPE;
extern const size_t TRIANGLE_OBSTACLE_TYPE;

/**
 * This function initializes the game map and is called once 
 *
 * @param scene the scene 
 */
void map_init(scene_t *scene);

#endif // #ifndef __STAR_H__
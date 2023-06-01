#ifndef __MENU_H__
#define __MENU_H__

#include "color.h"
#include "list.h"
#include "scene.h"
#include "state.h"
#include "vector.h"
#include <stdbool.h>
#include "sdl_wrapper.h"

extern const double MAX_WIDTH_GAME = 1600.0;
extern const double MAX_HEIGHT_GAME = 1300.0;

/**
 * This function initializes the main menu 
 *
 * *@param scene the scene to which bodies and 
 * graphics are added
 */
void menu_init(scene_t *scene);

#endif // #ifndef __MAIN_MENU_H__


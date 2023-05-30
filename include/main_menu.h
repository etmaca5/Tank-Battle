#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

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
 * Button struct represents an interactive button with text.
 */
typedef struct button button_t;

/**
 * This function determines whether a button has been pressed (if 
 * a given point that a mouse clicks is in the rectangle)
 * 
 * @param x The x-coordinate of the point.
 * @param y The y-coordinate of the point.
 * @param rect The rectangle to check against.
 * @return true if the point is inside the rectangle, false otherwise.
 */
bool button_presssed(int x, int y, SDL_Rect rect)

/**
 * This function initializes the main menu before the game begins 
 *
 * @param body the body to check
 */
void main_menu();

#endif // #ifndef __MAIN_MENU_H__

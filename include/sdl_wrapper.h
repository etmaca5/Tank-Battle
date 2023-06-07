#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include "color.h"
#include "list.h"
#include "scene.h"
#include "state.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "rect.h"

// Values passed to a key handler when the given arrow key is pressed
typedef enum {
  LEFT_ARROW = 1,
  UP_ARROW = 2,
  RIGHT_ARROW = 3,
  DOWN_ARROW = 4,
  SPACE = 5,
  MOUSE_CLICK = 6,
  MOUSE_MOVED = 7,
} arrow_key_t;

// typedef struct text {
//   char *words;
//   SDL_Rect dest;
//   SDL_Color color;
//   int font_size;
// } text_t; 

typedef struct image {
    SDL_Texture *texture;
    int width;
    int height;
} image_t; 


image_t sdl_load_image(char *filename);
image_t sdl_draw_text(char *text);

void sdl_draw_image(image_t *img, rect_t rect);
vector_t sdl_mouse_position();

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum { KEY_PRESSED, KEY_RELEASED, MOUSE_ENGAGED } key_event_type_t;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 */
typedef void (*key_handler_t)(char key, key_event_type_t type, double held_time,
                              state_t *state, vector_t loc);

/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(vector_t min, vector_t max);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle keypresses.
 *
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done(state_t *state);

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param points the list of vertices of the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(list_t *points, rgb_color_t color);

// /**
//  * Draws a polygon from the given list of vertices and a color.
//  *
//  * @param graphic the graphic to draw
//  * @param loc the location of where to draw the graphic
//  */
// void sdl_draw_graphic(graphic_t *graphic, vector_t loc);

// /**
//  * Loads a graphic from a file and displays it on the screen.
//  * The file should be in a supported format, such as PNG, JPEG, or BMP.
//  * 
//  * @param filename The filename of the image  
//  */
// graphic_t *sdl_load_graphic(char *filename);

// /**
//  * Text The text to be rendered as a graphic to display on screen. 
//  * 
//  * @param text The text to be displayed on screen. 
//  */
// void sdl_load_text(text_t *text);

/**
 * Function returns vector_t of mouse position 
 */
vector_t sdl_mouse_position();

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 */
void sdl_render_scene(scene_t *scene);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, key_event_type_t type, double held_time) {
 *     if (type == KEY_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 printf("A pressed\n");
 *                 break;
 *             case UP_ARROW:
 *                 printf("UP pressed\n");
 *                 break;
 *         }
 *     }
 * }
 * int main(void) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 */
void sdl_on_key(key_handler_t handler);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

vector_t get_window_position(vector_t scene_pos, vector_t window_center);

double get_scene_scale(vector_t window_center);

vector_t get_window_center(void);

#endif // #ifndef __SDL_WRAPPER_H__

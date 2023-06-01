#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "scene.h"
#include "list.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_wrapper.h"
#include "color.h"
#include "state.h"
#include "vector.h"
#include <stdbool.h>

list_t *make_rectangle(vector_t corner, double width, double height) {
    list_t *rectangle = list_init(4, (free_func_t)free);
    vector_t *point1 = malloc(sizeof(vector_t));
    assert(point1 != NULL);
    point1->x = corner.x;
    point1->y = corner.y;
    list_add(rectangle, point1);
    vector_t *point2 = malloc(sizeof(vector_t));
    assert(point2 != NULL);
    point2->x = corner.x;
    point2->y = corner.y - height;
    list_add(rectangle, point2);
    vector_t *point3 = malloc(sizeof(vector_t));
    assert(point3 != NULL);
    point3->x = corner.x + width;
    point3->y = corner.y - height;
    list_add(rectangle, point3);
    vector_t *point4 = malloc(sizeof(vector_t));
    assert(point4 != NULL);
    point4->x = corner.x + width;
    point4->y = corner.y;
    list_add(rectangle, point4);
    return rectangle;
}

list_t *make_circle(vector_t center, double radius) {
  list_t *ball = list_init(CIRCLE_POINTS, (free_func_t)free);

  for (size_t i = 0; i < CIRCLE_POINTS; i++) {
    vector_t *point = malloc(sizeof(vector_t));
    assert(point != NULL);

    point->x = center.x;
    point->y = center.y + radius;

    list_add(ball, point);
    polygon_rotate(ball, -M_PI / (CIRCLE_POINTS / 2), center);
  }
  return ball;
}

void menu_handler(char key, key_event_type_t type, double held_time, state_t *state, vector_t loc) {
  










}

// add buttons to scene (start, tank options, exit)
//make sure that handler recognizes where the position is
//draw the bodies 

//scene draw polygon and draw graphic (overlay them)
//scene draw image (can be text) 
//scene draw

//sdl_on_key(mouse_handler);

void menu_init(scene_t *scene) {
    //load all the text  
    SDL_Color title_color = { 0, 0, 0, 255 };
    graphic_t *main_menu_title = sdl_load_text("Main Menu",150, title_color);
    vector_t title_loc = { MAX_WIDTH_GAME / 4, MAX_HEIGHT_GAME - MAX_HEIGHT_GAME / 8};
    sdl_draw_graphic(main_menu_title, title_loc);

    SDL_Color start_color = { 0, 0, 0, 255 };
    graphic_t *start_button = sdl_load_text("Start!", 100, start_color);
    vector_t start_loc = {MAX_WIDTH_GAME / 2, MAX_HEIGHT_GAME / 2 + 100.0};
    sdl_draw_graphic(start_button, start_loc);
    
    SDL_Color options_color = { 0, 0, 0, 255 };
    graphic_t *tank_options = sdl_load_text("Select Tank", 80, options_color);
    vector_t tank_options_loc = {MAX_WIDTH_GAME / 5, MAX_HEIGHT_GAME / 5};
    sdl_draw_graphic(tank_options, tank_options_loc);

    graphic_t *exit = sdl_load_text("Exit");
    vector_t exit_loc = {4 * MAX_WIDTH / 5, MAX_HEIGHT_GAME / 5};
    sdl_draw_graphic(exit, exit_loc);

    vector_t start_loc = { MAX_WIDTH_GAME / 2, MAX_HEIGHT_GAME / 2}
    //load the buttons -> polygons
    list_t *start_button = make_circle(start_loc, 100.0);
    rgb_color_t green = {0, 1, 0};
    sdl_draw_polygon(start_button, green);

    //load the buttons -> polygons
    list_t *options_button = make_rectangle(tank_options_loc, 160.0, 100.0);
    rgb_color_t light_gray = {0.86, 0.86, 0.86};
    sdl_draw_polygon(options_button, light_gray);

    list_t *exit_button = make_rectangle(exit_loc, 160.0, 100.0);
    sdl_draw_polygon(exit_button, light_gray);


    
    //draw scene 
    sdl_show();
}
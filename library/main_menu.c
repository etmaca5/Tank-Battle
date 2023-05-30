#include "main_menu.h"
#include "sdl_wrapper.h"
#include "state.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// start button dimensions
int START_BUTTON_WIDTH = 500;
int START_BUTTON_HEIGHT = 200;
int START_BUTTON_ID = 1;

// option button dimensions
int OPTIONS_BUTTON_WIDTH = 350;
int OPTIONS_BUTTON_HEIGHT = 100;
int OPTIONS_BUTTON_ID = 2;

// exit button dimensions
int EXIT_BUTTON_WIDTH = 350;
int EXIT_BUTTON_HEIGHT = 100;
int EXIT_BUTTON_ID = 3;

typedef struct {
  SDL_Rect rectangle;
  int id;
  const char *text;
} button;

bool button_pressed(int x, int y, SDL_Rect rect) {
  return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y &&
          y <= rect.y + rect.h);
}

void main_menu {
  SDL_Window *window = SDL_CreateWindow("Main Menu", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, MAX_WIDTH_GAME,
                                        MAX_HEIGHT_GAME, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Event event;
  bool running = true;

  button_t *start_button = malloc(sizeof(button_t));
  start_button->rectangle = {(int)MAX_WIDTH_GAME / 4,
                             (int)(MAX_HEIGHT_GAME - MAX_HEIGHT_GAME / 6),
                             START_BUTTON_WIDTH, START_BUTTON_HEIGHT};
  start_button->id = START_BUTTON_ID;
  start_button->text = "Start Game";

  button_t *options_button = malloc(sizeof(button_t));
  options_button->rectangle = {(int)MAX_WIDTH_GAME / 10,
                               (int)MAX_HEIGHT_GAME / 3, OPTIONS_BUTTON_WIDTH,
                               OPTIONS_BUTTON_HEIGHT};
  options_button->id = START_BUTTON_ID;
  options_button->text = "Options";

  button_t *exit_button = malloc(sizeof(button_t));
  options_button->rectangle = {(int)3 * MAX_WIDTH_GAME / 5,
                               (int)MAX_HEIGHT_GAME / 3, EXIT_BUTTON_WIDTH,
                               EXIT_BUTTON_HEIGHT};
  options_button->id = START_BUTTON_ID;
  options_button->text = "Exit";

  while (running) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          int x = event.button.x;
          int y = event.button.y;
          if (button_pressed(x, y, start_button->rectangle)) {
            // close the main menu
          } else if (button_pressed(x, y, options_button->rectangle)) {
            // open options window to change tanks
          } else if (button_pressed(x, y, exit_button->rectangle)) {
            // exit game
          }
        }
        break;
      }
    }
  }

  TTF_Init();
  if (TTF_Init() != 0) {
    printf("TTF_Init Error: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }
  // don't know if need these lines
  // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  // SDL_RenderClear(renderer);

  // start button green
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  SDL_RenderFillRect(renderer, &start_button->rectangle);

  // options and exit button grey
  SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
  SDL_RenderFillRect(renderer, &options_button->rectangle);
  SDL_RenderFillRect(renderer, &exit_button->rectangle);

  SDL_Color text_color = {0, 0, 0, 255};
  TTF_Font *font = TTF_OpenFont("lato.ttf", 28);

  SDL_Surface *start_surface =
      TTF_RenderText_Solid(font, start_button->text, text_color);
  SDL_Texture *start_texture =
      SDL_CreateTextureFromSurface(renderer, start_surface);
  SDL_RenderCopy(renderer, start_texture, NULL, &start_button->rectangle);
  SDL_FreeSurface(start_surface);
  SDL_DestroyTexture(start_texture);

  SDL_Surface *options_surface =
      TTF_RenderText_Solid(font, options_button->text, text_color);
  SDL_Texture *options_texture =
      SDL_CreateTextureFromSurface(renderer, options_surface);
  SDL_RenderCopy(renderer, options_texture, NULL, &options_button->rectangle);
  SDL_FreeSurface(options_surface);
  SDL_DestroyTexture(options_texture);

  SDL_Surface *exit_surface =
      TTF_RenderText_Solid(font, exit_button->text, text_color);
  SDL_Texture *exit_texture =
      SDL_CreateTextureFromSurface(renderer, exit_surface);
  SDL_RenderCopy(renderer, exit_texture, NULL, &exit_button->rectangle);
  SDL_FreeSurface(exit_surface);
  SDL_DestroyTexture(exit_texture);

  // free
  TTF_CloseFont(font);
  TTF_Quit();
  free(start_button);
  free(options_button);
  free(exit_button);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
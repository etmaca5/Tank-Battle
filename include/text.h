#ifndef __TEXT_H__
#define __TEXT_H__

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "list.h"
#include <SDL2/SDL_ttf.h>

typedef struct text text_t;

text_t *text_init(TTF_Font *font, free_func_t text_free);

void text_free(text_t *text);

TTF_Font *text_get_font(text_t *text);

#endif // #ifndef __TEXT_H__
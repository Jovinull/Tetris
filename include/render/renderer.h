#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct Renderer {
  SDL_Window* win;
  SDL_Renderer* r;

  int logical_w;
  int logical_h;

  int shake_ms;
  int shake_px;
} Renderer;

typedef struct ColorRGBA {
  Uint8 r, g, b, a;
} ColorRGBA;

bool renderer_init(Renderer* rd, const char* title, int logical_w, int logical_h, int fullscreen, int scale);
void renderer_shutdown(Renderer* rd);

void renderer_begin(Renderer* rd);
void renderer_present(Renderer* rd);

void renderer_set_shake(Renderer* rd, int ms, int px);

void draw_rect(Renderer* rd, int x, int y, int w, int h, ColorRGBA c, bool filled);
void draw_text(Renderer* rd, int x, int y, int scale, ColorRGBA c, const char* text);
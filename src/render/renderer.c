#include "render/renderer.h"
#include "render/font5x7.h"
#include "utils/str.h"
#include "utils/log.h"
#include <string.h>

static void set_color(SDL_Renderer* r, ColorRGBA c) {
  (void)SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
}

bool renderer_init(Renderer* rd, const char* title, int logical_w, int logical_h, int fullscreen, int scale) {
  if (!rd || !title) return false;
  rd->win = NULL;
  rd->r = NULL;
  rd->logical_w = logical_w;
  rd->logical_h = logical_h;
  rd->shake_ms = 0;
  rd->shake_px = 0;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
    log_error("SDL_Init failed: %s", SDL_GetError());
    return false;
  }

  const Uint32 wflags = SDL_WINDOW_SHOWN | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0u);
  const int win_w = logical_w * clamp_i(scale, 2, 6);
  const int win_h = logical_h * clamp_i(scale, 2, 6);

  rd->win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h, wflags);
  if (!rd->win) {
    log_error("SDL_CreateWindow failed: %s", SDL_GetError());
    return false;
  }

  rd->r = SDL_CreateRenderer(rd->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!rd->r) {
    log_error("SDL_CreateRenderer failed: %s", SDL_GetError());
    return false;
  }

  (void)SDL_RenderSetLogicalSize(rd->r, rd->logical_w, rd->logical_h);
  (void)SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
  return true;
}

void renderer_shutdown(Renderer* rd) {
  if (!rd) return;
  if (rd->r) { SDL_DestroyRenderer(rd->r); rd->r = NULL; }
  if (rd->win) { SDL_DestroyWindow(rd->win); rd->win = NULL; }
  SDL_Quit();
}

void renderer_set_shake(Renderer* rd, int ms, int px) {
  if (!rd) return;
  rd->shake_ms = clamp_i(ms, 0, 400);
  rd->shake_px = clamp_i(px, 0, 12);
}

static void apply_shake(Renderer* rd) {
  if (!rd || rd->shake_ms <= 0) return;
  const int dx = (int)(SDL_GetTicks() % 7u) - 3;
  const int dy = (int)((SDL_GetTicks() / 3u) % 7u) - 3;
  SDL_Rect vp = { dx, dy, rd->logical_w, rd->logical_h };
  SDL_RenderSetViewport(rd->r, &vp);
}

static void decay_shake(Renderer* rd) {
  if (!rd || rd->shake_ms <= 0) return;
  rd->shake_ms -= 16; // aproximado (vsync)
  if (rd->shake_ms < 0) rd->shake_ms = 0;
  if (rd->shake_ms == 0) SDL_RenderSetViewport(rd->r, NULL);
}

void renderer_begin(Renderer* rd) {
  if (!rd || !rd->r) return;
  set_color(rd->r, (ColorRGBA){ 14, 14, 18, 255 });
  (void)SDL_RenderClear(rd->r);
  apply_shake(rd);
}

void renderer_present(Renderer* rd) {
  if (!rd || !rd->r) return;
  SDL_RenderPresent(rd->r);
  decay_shake(rd);
}

void draw_rect(Renderer* rd, int x, int y, int w, int h, ColorRGBA c, bool filled) {
  if (!rd || !rd->r) return;
  set_color(rd->r, c);
  SDL_Rect rc = { x, y, w, h };
  if (filled) (void)SDL_RenderFillRect(rd->r, &rc);
  else (void)SDL_RenderDrawRect(rd->r, &rc);
}

void draw_text(Renderer* rd, int x, int y, int scale, ColorRGBA c, const char* text) {
  if (!rd || !rd->r || !text) return;
  scale = clamp_i(scale, 1, 6);
  int cx = x;
  for (size_t i = 0u; text[i]; ++i) {
    const char ch = text[i];
    if (ch == '\n') { y += 8 * scale; cx = x; continue; }
    const Font5x7Glyph* g = font5x7_get(ch);
    for (int row = 0; row < 7; ++row) {
      const uint8_t bits = g->rows[row];
      for (int col = 0; col < 5; ++col) {
        if ((bits >> (4 - col)) & 1u) {
          draw_rect(rd, cx + col * scale, y + row * scale, scale, scale, c, true);
        }
      }
    }
    cx += 6 * scale;
  }
}
#include "ui/hud.h"
#include "utils/str.h"
#include <stdio.h>

static ColorRGBA color_for_cell(uint8_t v) {
  switch (v) {
    case 1: return (ColorRGBA){  80, 220, 220, 255 }; // I
    case 2: return (ColorRGBA){ 220, 220,  80, 255 }; // O
    case 3: return (ColorRGBA){ 180,  80, 220, 255 }; // T
    case 4: return (ColorRGBA){  80, 220, 120, 255 }; // S
    case 5: return (ColorRGBA){ 220,  80,  80, 255 }; // Z
    case 6: return (ColorRGBA){  80, 120, 220, 255 }; // J
    case 7: return (ColorRGBA){ 220, 140,  60, 255 }; // L
    default:return (ColorRGBA){  40,  40,  55, 255 };
  }
}

static void draw_cell(Renderer* rd, int x, int y, int cell, ColorRGBA c, bool outline) {
  draw_rect(rd, x, y, cell, cell, c, true);
  if (outline) draw_rect(rd, x, y, cell, cell, (ColorRGBA){0,0,0,80}, false);
}

static void draw_mini_piece(Renderer* rd, Tetromino t, int x, int y, int s) {
  Piece p; piece_spawn(&p, t);
  p.x = 0; p.y = 0;
  Cell c[4];
  piece_cells(&p, c);
  for (int i = 0; i < 4; ++i) {
    draw_cell(rd, x + (c[i].x + 2) * s, y + (c[i].y + 2) * s, s, color_for_cell((uint8_t)(t + 1)), true);
  }
}

void hud_render(Renderer* rd, const Gameplay* g, int origin_x, int origin_y, int cell) {
  if (!rd || !g) return;

  // board frame
  const int bw = BOARD_W * cell;
  const int bh = BOARD_H_VISIBLE * cell;
  draw_rect(rd, origin_x - 2, origin_y - 2, bw + 4, bh + 4, (ColorRGBA){90,90,110,255}, false);

  // draw placed cells (skip hidden rows)
  for (int y = BOARD_H_HIDDEN; y < BOARD_H; ++y) {
    for (int x = 0; x < BOARD_W; ++x) {
      const uint8_t v = g->board.cells[y][x];
      if (v) {
        draw_cell(rd, origin_x + x * cell, origin_y + (y - BOARD_H_HIDDEN) * cell, cell, color_for_cell(v), true);
      } else {
        // subtle grid
        draw_rect(rd, origin_x + x * cell, origin_y + (y - BOARD_H_HIDDEN) * cell, cell, cell, (ColorRGBA){25,25,35,255}, false);
      }
    }
  }

  // ghost
  if (g->opt.ghost && g->has_cur) {
    const int drop = gameplay_get_ghost_drop(g);
    Piece ghost = g->cur;
    ghost.y += drop;
    Cell gc[4]; piece_cells(&ghost, gc);
    for (int i = 0; i < 4; ++i) {
      const int gx = gc[i].x;
      const int gy = gc[i].y;
      if (gy >= BOARD_H_HIDDEN) {
        draw_rect(rd,
          origin_x + gx * cell,
          origin_y + (gy - BOARD_H_HIDDEN) * cell,
          cell, cell,
          (ColorRGBA){200,200,200,60}, false
        );
      }
    }
  }

  // current piece
  if (g->has_cur) {
    Cell c[4]; piece_cells(&g->cur, c);
    for (int i = 0; i < 4; ++i) {
      const int px = c[i].x;
      const int py = c[i].y;
      if (py >= BOARD_H_HIDDEN) {
        draw_cell(rd, origin_x + px * cell, origin_y + (py - BOARD_H_HIDDEN) * cell, cell,
                  color_for_cell((uint8_t)(g->cur.t + 1)), true);
      }
    }
  }

  // side HUD
  char buf[64];
  const ScoreState* st = g->score->get(g->score);

  draw_text(rd, origin_x + bw + 14, origin_y, 2, (ColorRGBA){230,230,240,255}, "SCORE");
  (void)snprintf(buf, sizeof(buf), "%d", st->score);
  draw_text(rd, origin_x + bw + 14, origin_y + 18, 2, (ColorRGBA){230,230,240,255}, buf);

  draw_text(rd, origin_x + bw + 14, origin_y + 46, 2, (ColorRGBA){230,230,240,255}, "LEVEL");
  (void)snprintf(buf, sizeof(buf), "%d", st->level);
  draw_text(rd, origin_x + bw + 14, origin_y + 64, 2, (ColorRGBA){230,230,240,255}, buf);

  draw_text(rd, origin_x + bw + 14, origin_y + 92, 2, (ColorRGBA){230,230,240,255}, "LINES");
  (void)snprintf(buf, sizeof(buf), "%d", st->lines_total);
  draw_text(rd, origin_x + bw + 14, origin_y + 110, 2, (ColorRGBA){230,230,240,255}, buf);

  draw_text(rd, origin_x + bw + 14, origin_y + 140, 2, (ColorRGBA){230,230,240,255}, "NEXT");
  for (int i = 0; i < 5; ++i) {
    draw_mini_piece(rd, g->next[i], origin_x + bw + 18, origin_y + 160 + i * 28, 6);
  }

  draw_text(rd, origin_x - 78, origin_y, 2, (ColorRGBA){230,230,240,255}, "HOLD");
  if (g->opt.hold_enabled && g->has_hold) {
    draw_mini_piece(rd, g->hold, origin_x - 72, origin_y + 22, 7);
  } else {
    draw_text(rd, origin_x - 72, origin_y + 26, 2, (ColorRGBA){150,150,170,255}, "OFF");
  }
}
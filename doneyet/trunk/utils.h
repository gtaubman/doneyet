#ifndef __UTILS__
#define __UTILS__

#include <ncurses.h>

typedef struct {
  int origin_x, origin_y;
  int width, height;
} window_info;

// Returns the origin and dimensions of an ncurses window.
window_info get_window_info(WINDOW* win);

#endif

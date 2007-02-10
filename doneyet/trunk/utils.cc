#include "utils.h"

// Returns the origin and dimensions of an ncurses window.
window_info get_window_info(WINDOW* win) {
  // The struct to return;
  window_info info;

  // Get the origin of the window
  int x, y;
  getyx(win, y, x);
  info.origin_x = x;
  info.origin_y = y;

  // Get the size of the window
  getmaxyx(win, y, x);
  info.width = x;
  info.height = y;

  return info;
}


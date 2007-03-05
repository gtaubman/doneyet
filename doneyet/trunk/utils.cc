#include <string.h>
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

void print_in_middle(WINDOW *win,
    int starty,
    int startx,
    int width,
    const char *string,
    chtype color) {
  int length, x, y;
  float temp;

  if(win == NULL)
    win = stdscr;
  getyx(win, y, x);
  if(startx != 0)
    x = startx;
  if(starty != 0)
    y = starty;
  if(width == 0)
    width = 80;

  length = strlen(string);
  temp = (width - length)/ 2;
  x = startx + (int)temp;
  wattron(win, color | A_BOLD);
  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color | A_BOLD);

  // Draw a horizontal line
  wmove(win, 2, 0);
  waddch(win, ACS_LTEE);
  for (int i = 1; i < width - 1; ++i) {
    waddch(win, ACS_HLINE);
  }
  waddch(win, ACS_RTEE);
  refresh();
}

int winheight(WINDOW* win) {
  int w,h;
  getmaxyx(win, h, w);
  return h;
}

int winwidth(WINDOW* win) {
  int w,h;
  getmaxyx(win, h, w);
  return w;
}

int chars_to_whitespace(const string& str, int i) {
  int p = str.find(" ", i);
  return p == string::npos ? str.size() - i : p - i;
}

void trim_multiple_spaces(string& str) {
  int space = 0;
  while (true) {
    space = str.find_first_of(" ", space);
    if (space == string::npos) {
      return;
    }
    int next_char = str.find_first_not_of(" ", space);
    ++space;
    str.erase(space, next_char - space);
    ++space;
  }
}

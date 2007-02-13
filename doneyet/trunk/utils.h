#ifndef __UTILS__
#define __UTILS__

#include <ncurses.h>
#include <string>

using std::string;

typedef struct {
  int origin_x, origin_y;
  int width, height;
} window_info;

// Returns the origin and dimensions of an ncurses window.
window_info get_window_info(WINDOW* win);
void print_in_middle(WINDOW *win,
    int starty,
    int startx,
    int width,
    const char *string,
    chtype color);

int winheight(WINDOW* win);
int winwidth(WINDOW* win);

int chars_to_whitespace(const string& str, int i);

#endif

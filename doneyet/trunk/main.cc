#include <curses.h>
#include <menu.h>
#include "project.h"

int main(int argc, char** argv) {
  initscr();  // Create the standard window.
  keypad(stdscr, true);        // Enable keyboard mappings
  nonl();                         // Disable weird newline stuff.
  cbreak();                       // Take input characters one at a time.
  noecho();                       // We don't want input to be echoed

  // If our terminal has color, start up colors
  if (has_colors()) {
    start_color();
    
    // Make some color pairs
    init_pair(1, COLOR_RED,     COLOR_BLACK);
    init_pair(2, COLOR_GREEN,   COLOR_BLACK);
    init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
    init_pair(4, COLOR_BLUE,    COLOR_BLACK);
    init_pair(5, COLOR_CYAN,    COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_WHITE,   COLOR_BLACK);
  }

  wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');
  Project* p = Project::NewProject();

  endwin();
  if (p) {
    printf("NEW PROJECT: \"%s\"\n", p->name_.c_str());
  } else {
    printf("hit escape\n");
  }
  return 0;
}

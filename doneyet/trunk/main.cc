#include <curses.h>
#include <menu.h>
#include "project.h"
#include "dialog-box.h"

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

  //wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');
  box(stdscr, 0, 0);
  Project* p = Project::NewProject();

  doupdate();
  if (p) {
    p->DrawInWindow(stdscr);
    getch();
    DialogBox::RunMultiLine( "Edit Task", p->tasks_[0]->title_,
        30, 1);
    printf("NEW PROJECT: \"%s\"\n", p->name_.c_str());
    refresh();
  } else {
    printf("hit escape\n");
  }
  endwin();
  return 0;
}

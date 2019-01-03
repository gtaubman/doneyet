#include <locale.h>
#include <curses.h>
#include <menu.h>
#include <iostream>
#include <fstream>
#include "workspace.h"
#include "doneyet-config.h"

using std::ofstream;
using std::iostream;

int main(int argc, char** argv) {
  if(argc>=2)
  {
        printf("%s does not understand any command line arguments yet.\nHere, have a help page :-)\n",argv[0]);
	string helptext =
"* A - Apply the Show All Tasks filter.\n"
"* a - Create a new task (or a subtask if pressed while a task is selected).\n"
"* M - Show the menu bar.\n"
"  * j and k - Change menu item.\n"
"  * l and h - Change menu.\n"
"  * Return - Select the selected menu item.\n"
"  * Escape - Hide the menu bar.\n"
"* m - Move the currently selected task. Note this doesn't work for root level tasks yet.\n"
"  * k/u/Up Arrow - Move selected task up.\n"
"  * j/d/Down Arrow - Move selected task down.\n"
"  * Return - Place task at current position.\n"
"  * Escape - Place task to where it was originally.\n"
"* n - Add a note to the selected task.\n"
"* v - View the notes of the selected task.\n"
"* j - Selected next task.\n"
"* k - Select previous task.\n"
"* Escape - Select no task.\n"
"* e - Edit selected task.\n"
"* d - Delete selected task.\n"
"* c - Toggle collapsed state of selected task.\n"
"* R - Apply the Show Uncompleted Tasks filter.\n"
"* C - Apply the Show Completed Tasks filter.\n"
"* f - Apply the Find Tasks filter.\n"
"* S - Save the project.\n"
"* Space - Toggle the status of the selected item. White is unstarted, green is in progress, blue is completed and red is paused.\n"
"* h - Shows and closes this help dialog.\n"
"* q - Quit.\n";
	printf("%s", helptext.c_str());
	return 0;
  }
  else
  {
  bool curses = true;

  DoneyetConfig* config = DoneyetConfig::GlobalConfig();
  if (config == NULL) {
    fprintf(stderr, "Unable to parse config file.\n");
    return 1;
  }

  if (curses) {
    setlocale(LC_ALL, "");
    initscr();  // Create the standard window.
    keypad(stdscr, true);        // Enable keyboard mappings
    nonl();                         // Disable weird newline stuff.
    cbreak();                       // Take input characters one at a time.
    noecho();                       // We don't want input to be echoed
    intrflush(stdscr, FALSE);


    // If our terminal has color, start up colors
    if (has_colors()) {
      start_color();

      if (assume_default_colors(config->ForegroundColor(),
                                config->BackgroundColor()) != OK) {
        fprintf(stderr, "Unable to set default colors.\n");
        endwin();
        return 1;
      }

      // Make some color pairs
      // TODO: Make the task drawing know which color pairs to use.
      short background_color = config->BackgroundColor();
      init_pair(1, COLOR_RED,     background_color);
      init_pair(2, COLOR_GREEN,   background_color);
      init_pair(3, COLOR_YELLOW,  background_color);
      init_pair(4, COLOR_BLUE,    background_color);
      init_pair(5, COLOR_CYAN,    background_color);
      init_pair(6, COLOR_MAGENTA, background_color);
      init_pair(7, COLOR_WHITE,   background_color);
      init_pair(8, COLOR_YELLOW,   COLOR_BLUE);
      init_pair(9, COLOR_YELLOW,   COLOR_BLUE);
    }
  }

  if (curses) {
    Workspace w;
    endwin();
  }

  delete config;
  return 0;
  }
}

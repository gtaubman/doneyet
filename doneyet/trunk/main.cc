#include <curses.h>
#include <menu.h>
#include <iostream>
#include <fstream>
#include "project.h"
#include "dialog-box.h"
#include "file-manager.h"
#include "serializer.h"

using std::ofstream;
using std::iostream;

int main(int argc, char** argv) {
  bool curses = true;
  FileManager* fm = FileManager::DefaultFileManager();
  if (curses) {
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

    box(stdscr, 0, 0);
  }

  Project* p = NULL;
  if (fm->NumSavedProjects() == 1) {
    printf("There was one saved project.\n");
    p = Project::NewProjectFromFile(fm->ProjectDir() + fm->SavedProjectNames()[0]);
  } else {
    p = Project::NewProject();
  }

  if (curses) {
    if (p) {
      p->DrawInWindow(stdscr);
      refresh();
    } else {
      printf("hit escape\n");
    }
    endwin();
  }

  std::string filename = fm->ProjectDir() + p->Name();  
  Serializer s("", filename);
  p->Serialize(&s);
  s.CloseAll();
  return 0;
}

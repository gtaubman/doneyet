#include <locale.h>
#include <ncursesw/menu.h>
#include <ncursesw/ncurses.h>

#include "doneyet-config.h"
#include "workspace.h"

int main(int argc, char** argv) {
  if (argc >= 2) {  // at least one argument or more
    printf(
        "%s does not understand any command line arguments yet.\nHere, have a "
        "help page :-)\n",
        argv[0]);
    printf("%s", __HELPTEXT__);
    return 0;
  } else {  // no arguments supplied
    DoneyetConfig* config = DoneyetConfig::GlobalConfig();
    if (config == NULL) {
      fprintf(stderr, "Unable to parse config file.\n");
      return 1;
    }

    setlocale( LC_ALL, "");    // setting locale helps drawing (still not entirely sure why)
    std::locale::global(std::locale(""));
    initscr();  // Create the standard window.
    keypad(stdscr, true);  // Enable keyboard mappings
    nonl();                // Disable weird newline stuff.
    cbreak();              // Take input characters one at a time.
    noecho();              // We don't want input to be echoed
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
      init_pair(1, COLOR_RED, background_color);
      init_pair(2, COLOR_GREEN, background_color);
      init_pair(3, COLOR_YELLOW, background_color);
      init_pair(4, COLOR_BLUE, background_color);
      init_pair(5, COLOR_CYAN, background_color);
      init_pair(6, COLOR_MAGENTA, background_color);
      init_pair(7, COLOR_WHITE, background_color);
      init_pair(8, COLOR_YELLOW, COLOR_BLUE);
      init_pair(9, COLOR_YELLOW, COLOR_BLUE);
    }

    Workspace w;
    endwin();

    delete config;
    return 0;
  }
}

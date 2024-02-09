#include <locale.h>
#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>

#include <locale>

#include "dialog-box.h"

int main() {
  setlocale(LC_ALL, "");
  std::locale::global(std::locale(""));

  /* Initialize curses */
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  string answer = DialogBox::RunMultiLine("Amözing title", "ödefault", 40, 10);
  mvprintw(10, 0, "Got %lu bytes\n", answer.length());
  mvprintw(11, 0, "Got %s\n", answer.c_str());
  getch();
  endwin();
  return 0;
}

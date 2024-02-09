#include <locale.h>
#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>

#include <string>

#include "file-versions.h"
#include "project.h"

int main() {
  setlocale(LC_ALL, "");
  /* Initialize curses */
  initscr();
  cbreak();
  // noecho();
  keypad(stdscr, TRUE);

  std::string input;
  refresh();
  Project* p = new Project("this");
  Serializer* s = new Serializer("", "./bla");
  s->SetVersion(NOTES_VERSION);
  p->AddTaskNamed("this2");
  // p->AddTaskNamed("😎 new öäü");
  p->Serialize(s);
  s->CloseAll();
  delete p;
  delete s;

  // int ch;
  // ch = getch();
  endwin();
  return 0;
}

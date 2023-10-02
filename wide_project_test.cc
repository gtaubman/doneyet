#define _XOPEN_SOURCE_EXTENDED
#include <locale.h>
#include <ncursesw/form.h>
#include <string>
#include "project.h"
#include "file-versions.h"

int main() {
  setlocale(LC_ALL, "");
  /* Initialize curses */
  initscr();
  cbreak();
  //noecho();
  keypad(stdscr, TRUE);

  std::wstring input;
  int ok=true;
  wint_t ch;
  mvaddwstr(0,0,L"Give me a project name:");
  while ( ch != '\n' ) {
      ok = get_wch(&ch);
      input.push_back(ch);
  }
  //printw("%ls\n", ch);
  mvaddwstr(1,0,input.c_str());
  refresh();
  Project *p =  new Project("bla");
  p->AddTaskNamed("new task");
    Serializer s("", "./bla");
    s.SetVersion(NOTES_VERSION);
    p->Serialize(&s);
    s.CloseAll();

  ok = get_wch(&ch);
  endwin();
  return 0;
}

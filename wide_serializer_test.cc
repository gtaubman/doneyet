#include <locale.h>
#include <ncurses/form.h>
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

  std::string input;
  int ok=true;
  int ch;
  mvaddstr(0,0,"Give me a project name:");

  while ( ch != '\n' ) {
      ch = getch();
      input.push_back(ch);
  }

  mvaddstr(1,0,input.c_str());
  refresh();
  Project *p =  new Project(input);
  Serializer* s = new Serializer("", "./bla");
  s->SetVersion(NOTES_VERSION);
  p->AddTaskNamed("new task meet with fiancé");
  p->AddTaskNamed("cool new öäü");
  p->Serialize(s);
  s->CloseAll();
  delete p;
  delete s;

  ch = getch();
  endwin();
  return 0;
}

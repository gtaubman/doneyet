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
  //Project *p =  new Project(L"MyProject");
  //p->AddTaskNamed(L"new task meet with fiancé at ÂÂ°Albörg");
  Serializer s("", "./bla");
  //  p->Serialize(&s);
  s.SetVersion(NOTES_VERSION);
  s.WriteString(L"input öäü fiancé: ÂÄ many more words");
  s.WriteString(input);
  s.Flush();
  s.CloseAll();
  //delete p;
  Serializer s2("./bla", "./bla2");
  s2.SetVersion(NOTES_VERSION);
  //mvaddwstr(5,0,s2.ReadString().c_str());
  //mvaddwstr(6,0,s2.ReadString().c_str());
  std::wstring res = s2.ReadString();
  printw("%ls : %08x\n", res.c_str(), res.c_str());
  res = s2.ReadString();
  printw("%ls : %08x\n", res.c_str(), res.c_str());
  s2.CloseAll();

  ok = get_wch(&ch);
  endwin();
  return 0;
}

#define _XOPEN_SOURCE_EXTENDED
#include <locale.h>
#include <ncursesw/form.h>

int main() {
  setlocale(LC_ALL, "");
  /* Initialize curses */
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  /* Create the form and post it */
  // wchar_t wstr[] = { 9474, L'\0' };
  // mvaddwstr(0, 0, wstr);
  mvprintw(1, 0, "Input tester\n");
  mvprintw(2, 0, "Press DOWN KEY to abort\n");
  refresh();

  int ok;
  wint_t ch;
  for (;;) {
    ok = get_wch(&ch);
    if (ch == KEY_DOWN) {
      break;
    } else {
      printw("%lc\n", ch);
    }
    refresh();
  }
  endwin();
  return 0;
}

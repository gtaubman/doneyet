#include <locale.h>
#include <ncursesw/form.h>

int main() {
  setlocale(LC_ALL, "");
  /* Initialize curses */
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  // wchar_t wstr[] = { 9474, L'\0' };
  // mvaddwstr(0, 0, wstr);
  mvprintw(1, 0, "Input tester\n");
  mvprintw(2, 0, "Press DOWN KEY to abort\n");
  refresh();

  int ch;
  for (;;) {
    ch = getch();
    if (ch == KEY_DOWN) {
      break;
    } else {
      // printf("%ls \n", L"\U0001f600");
      printw("%d : %08x\n", ch, ch);
    }
    refresh();
  }
  endwin();
  return 0;
}

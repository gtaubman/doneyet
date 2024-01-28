#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <ncursesw/form.h>
#include "dialog-box.h"
#include <clocale>
#include <locale>

#undef CTRL
#define CTRL(x)	((x) & 0x1f)
#define ESCAPE		CTRL('[')

int main() {

    setlocale(LC_ALL, "");
    std::locale::global(std::locale(""));

    /* Initialize curses */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    wstring answer = DialogBox::RunMultiLine(L"Amözing title", L"ödefault", 40, 10);
    /*
    while (!done) {
        wint_t c2;
        int ret = get_wch(&c2);

        mvprintw(MyRow(NUM_FIELDS),
                 MyCol(NUM_FIELDS),
                 "Got %d (%#x), type: %s",
                 (int) c2,
                 (int) c2,
                 (ret == KEY_CODE_YES)
                 ? "KEY_CODE_YES"
                 : ((ret == OK)
                    ? "OK"
                    : ((ret == ERR)
                       ? "ERR"
                       : "?")));
        clrtoeol();

        switch (ret) {
            case KEY_CODE_YES: //we have a key code, so deal with that (not a wide char)
                switch (c2) {
                    case KEY_DOWN:
                        form_driver_w(my_form, KEY_CODE_YES, REQ_NEXT_FIELD);
                        form_driver_w(my_form, KEY_CODE_YES, REQ_END_LINE);
                        break;
                    case KEY_BTAB:
                    case KEY_UP:
                        form_driver_w(my_form, KEY_CODE_YES, REQ_PREV_FIELD);
                        form_driver_w(my_form, KEY_CODE_YES, REQ_END_LINE);
                        break;
                    default:
                        break;
                }
                break;
            case OK: // we have a wide char, deal with that
                switch (c2) {
                    case ESCAPE:
                        done = TRUE;
                        break;
                    case '\t':
                        form_driver_w(my_form, KEY_CODE_YES, REQ_NEXT_FIELD);
                        form_driver_w(my_form, KEY_CODE_YES, REQ_END_LINE);
                        break;
                    default:
                        form_driver_w(my_form, OK, (wchar_t) c2);
                        break;
                }
                break;
        }
    }
     */
    //clrtoeol();
    mvprintw(10,0, "Got %lu bytes\n",answer.length());
    mvprintw(11,0, "Got %ls\n", answer.c_str());
    getch();
    endwin();
    return 0;
}

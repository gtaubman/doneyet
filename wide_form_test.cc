#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <ncursesw/form.h>
#include <clocale>

#undef CTRL
#define CTRL(x)	((x) & 0x1f)
#define ESCAPE		CTRL('[')

int main(int argc, char *argv[]) {
    static const char *help[] =
            {
                    "Commands:",
                    "  ^D,^Q,ESC           - quit program",
                    "  <Tab>,<Down>        - move to next field",
                    "  <BackTab>,<Up>      - move to previous field",
                    0
            };

#define NUM_FIELDS 3
#define MyRow(n) (4 + (n) * 2)
#define MyCol(n) 10
    FIELD *field[NUM_FIELDS + 1];
    FORM *my_form;
    bool done = FALSE;
    int n;
    int ch;

    setlocale(LC_ALL, "");

    /* Initialize curses */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    /* Initialize the fields */
    for (n = 0; n < NUM_FIELDS; ++n) {
        field[n] = new_field(1, 10, MyRow(n), 18, 0, 0);
        set_field_back(field[n], A_UNDERLINE);
        /* Print a line for the option  */
        field_opts_off(field[n], O_AUTOSKIP);
        /* Don't go to next field when this is filled */
    }
    field[n] = NULL;

    /* Create the form and post it */
    my_form = new_form(field);
    post_form(my_form);
    refresh();

    for (n = 0; n < NUM_FIELDS; ++n) {
        mvprintw(MyRow(n), MyCol(n), "Value %d:", n + 1);
    }

    /* Loop through to get user requests */
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
            case KEY_CODE_YES:
                switch (c2) {
                    case KEY_DOWN:
                        /* Go to next field */
                        form_driver_w(my_form, KEY_CODE_YES, REQ_NEXT_FIELD);
                        /* Go to the end of the present buffer */
                        /* Leaves nicely at the last character */
                        form_driver_w(my_form, KEY_CODE_YES, REQ_END_LINE);
                        break;
                    case KEY_BTAB:
                    case KEY_UP:
                        /* Go to previous field */
                        form_driver_w(my_form, KEY_CODE_YES, REQ_PREV_FIELD);
                        form_driver_w(my_form, KEY_CODE_YES, REQ_END_LINE);
                        break;
                    default:
                        break;
                }
                break;
            case OK:
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

    /* Un post form and free the memory */
    unpost_form(my_form);
    free_form(my_form);
    for (n = 0; n < NUM_FIELDS; ++n) {
        free_field(field[n]);
    }

    endwin();
    return 0;
}

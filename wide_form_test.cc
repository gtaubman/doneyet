#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <ncursesw/form.h>
#include <clocale>

int main() {

    FIELD *fields[2];
    FORM *my_form;
    bool done = FALSE;

    setlocale(LC_ALL, "");

    /* Initialize curses */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    fields[0] = new_field(1, 10, 0, 18, 0, 0);
    set_field_back(fields[0], A_UNDERLINE);
    fields[1] = nullptr;

    /* Create the form and post it */
    my_form = new_form(fields);
    post_form(my_form);
    refresh();
    mvprintw(0, 0, "Value: ");

    /* Loop through to get user requests */
    while (!done) {
        wint_t c2;
        int ret = get_wch(&c2);
        int localrc=0;

        mvprintw(2, 2, "Got %d (%#x), type: %s",
                 (int) c2,
                 (int) c2,
                 (ret == KEY_CODE_YES)
                 ? "KEY_CODE_YES"
                 : ((ret == OK)
                    ? "OK"
                    : ((ret == ERR)
                       ? "ERR"
                       : "?")));
        mvprintw(3, 2, "Memory: %p", field_buffer(fields[0], 0));
        mvprintw(4, 2, "RC: %d", localrc);
        clrtoeol();

        if (ret == KEY_CODE_YES) { //we have a key code, so deal with that (not a wide char)
            if (c2 ==  KEY_DOWN){
                //localrc = form_driver_w(my_form, KEY_CODE_YES, REQ_END_LINE);
                localrc = form_driver_w(my_form, KEY_CODE_YES, REQ_VALIDATION);
                done = TRUE;
            }
        } else if (ret == OK) { //this is a wide char
            localrc = form_driver_w(my_form, OK, (wchar_t) c2);
        }
    }

    mvprintw(9,0, "Hello");
    int rc = form_driver_w(my_form, KEY_CODE_YES, REQ_VALIDATION);
    if (rc != E_OK) {
        mvprintw(10, 0, "Validation failed, got: %d", rc);
    }
    //wchar_t buffer[100];
    //memcpy(buffer, field_buffer(fields[0], 0), 100 );
    wchar_t * ptr = (wchar_t *) field_buffer(fields[0], 0);
    mvprintw(11,0, "Pointer: %p", ptr);
    mvprintw(12,0, "Printing test: %ls", L"testing baby");
    mvprintw(13,0, "Pointer Content: %ls", ptr );
    mvprintw(14,0, "Printing test: %ls", L"testing baby");

    getch();
    /* Un post form and free the memory */
    unpost_form(my_form);
    free_form(my_form);
    free_field(fields[0]);
    endwin();
    return 0;
}

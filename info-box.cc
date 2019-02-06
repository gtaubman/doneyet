#include "info-box.h"

void draw_borders(WINDOW *screen) {
    int x, y, i;

    getmaxyx(screen, y, x);

    mvwaddch(screen, 0, 0, ACS_ULCORNER);
    mvwaddch(screen, y - 1, 0, ACS_LLCORNER);
    mvwaddch(screen, 0, x - 1, ACS_URCORNER);
    mvwaddch(screen, y - 1, x - 1, ACS_LRCORNER);

    for (i = 1; i < (x - 1); i++) {
        mvwaddch(screen, 0, i, ACS_HLINE);
        mvwaddch(screen, y - 1, i, ACS_HLINE);
    }
    for (i = 1; i < (y - 1); i++) {
        mvwaddch(screen, i, 0, ACS_VLINE);
        mvwaddch(screen, i, x - 1, ACS_VLINE);
    }
}

void InfoBox::ShowFullScreen(const string& title,
                             const string& default_text,
                             int width,
                             int height) {
    WINDOW *field = newwin(height, width, 0, 0);
    draw_borders(field);
    WINDOW *fieldInner = newwin(height - 2, width - 2, 1, 1);
    mvwprintw(fieldInner, 0, 0, title.c_str());
    mvwprintw(fieldInner, 1, 0, default_text.c_str());
    wrefresh(field);
    wrefresh(fieldInner);
    getch();
    delwin(fieldInner);
    delwin(field);
}

void InfoBox::ShowMultiLine(const string& title,
                            const string& default_text,
                            int width,
                            int height) {
    // Create the text field
    FIELD* field = new_field(height, width, 0, 0, 0, 0);

    // Set its options
    field_opts_off(field, O_EDIT);
    if (!default_text.empty()) {
        set_field_buffer(field, 0, default_text.c_str());
    }

    // Make the usual list of fields
    FIELD* fields[] = { field, NULL };

    // Finally make our form
    FORM* form = new_form(fields);

    // Create a window big enough to hold our form.  Then make it slightly
    // bigger to allow space for borders and the title text.
    // Get the min size for the form:
    int rows, cols;
    scale_form(form, &rows, &cols);

    // Figure out how many lines of text we'll have for the label:
    int label_rows = 1;  //ceil(title.size() / cols);

    // Account for the two side borders, plus two spaces on either side of the
    // text entry box:
    int form_win_cols = cols + 2 + 4;

    // Account for the top and bottom borders, any label lines and the divider
    // between the label region and the text region.
    int form_win_rows = rows + 2 + label_rows + 1;

    // Get the dimensions of the main screen so we can put this new window in the
    // center of it.
    window_info info = CursesUtils::get_window_info(stdscr);

    // Create the outer form window
    WINDOW* form_win = newwin(form_win_rows,
                              form_win_cols,
                              info.height / 2 - form_win_rows / 2,
                              info.width / 2 - form_win_cols / 2);

    // Enable the arrow keys in our new window
    keypad(form_win, true);

    // Set the form's parent window to be this new window
    set_form_win(form, form_win);

    // Make the subwindow for the form itself
    WINDOW* subwindow = derwin(form_win, rows, cols, 2 + label_rows, 3);
    set_form_sub(form, subwindow);

    // Draw a box around our form's window
    //wborder(form_win, '|', '|', '-', '-', '+', '+', '+', '+');
    box(form_win, 0, 0);

    // Draw the label text
    CursesUtils::print_in_middle(form_win,
                                 1,
                                 0,
                                 form_win_cols,
                                 title.c_str(),
                                 COLOR_PAIR(1));

    // Show the form window
    post_form(form);
    wrefresh(form_win);

    bool done = false;
    while (!done) {
        int ch = wgetch(form_win);
        switch (ch) {
        case 'h':
        case 'q':
        case 27:  // escape
            done = true;
            break;
        case KEY_LEFT: // left arrow
            form_driver(form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT: // right arrow
            form_driver(form, REQ_NEXT_CHAR);
            break;
        case KEY_UP: // up arrow
            form_driver(form, REQ_PREV_LINE);
            break;
        case KEY_DOWN: // dn arrow
            form_driver(form, REQ_NEXT_LINE);
            break;
        }
    }

    // Without calling this the output doesn't actually get put in the buffer.
    form_driver(form, REQ_VALIDATION);

    // Free up our memory
    unpost_form(form);
    free_form(form);
    free_field(field);
    delwin(subwindow);
    delwin(form_win);
}

#include "list-chooser.h"
#include <stdlib.h>

string ListChooser::GetChoiceWithOptions(const vector<string>& choices,
        const string& message,
        int fg_color,
        int bg_color,
        int xloc,
        int yloc,
        bool border,
        string mark) {
    if (!choices.size()) {
        return "";
    }

    // Create the menu and its items:
    ITEM** items = (ITEM**) malloc((1 + choices.size()) * sizeof(ITEM*));
    items[choices.size()] = NULL;
    for (int i = 0; i < choices.size(); ++i) {
        items[i] = new_item(choices[i].c_str(), "");
    }

    // Create the menu itself:
    MENU* menu = new_menu(items);
    set_menu_fore(menu, fg_color);
    set_menu_back(menu, bg_color);

    // Create the windows to hold it:
    int rows, cols;
    scale_menu(menu, &rows, &cols);
    if (rows > CursesUtils::winheight(stdscr)) {
        rows = CursesUtils::winheight(stdscr) - 4 - 2 * border;
    }
    if (cols > CursesUtils::winwidth(stdscr)) {
        cols = CursesUtils::winwidth(stdscr) - 4 - 2 * border;
    }

    // Figure out the size of the window depending on whether or not we're drawing
    // a border:
    int spacing = 0;
    if (border) {
        spacing += 2;
    }

    // If there's a message to be displayed, then add an extra line to allow for it.
    int message_padding = message.size() ? 1 : 0;

    if (xloc == -1 && yloc == -1) {
        window_info info = CursesUtils::get_window_info(stdscr);
        xloc = info.width / 2 - (cols + spacing) / 2;
        yloc = info.height / 2 - (rows + spacing) / 2;
    }

    WINDOW* frill_window = newwin(rows + spacing + message_padding,
                                  cols + spacing + message.size(),
                                  yloc,
                                  xloc);
    keypad(frill_window, true);
    WINDOW* text_window = derwin(frill_window, rows,
                                 cols, border + message_padding, border);
    set_menu_win(menu, frill_window);
    set_menu_sub(menu, text_window);

    // Set the mark for the menu:
    set_menu_mark(menu, mark.c_str());

    // Begin drawing
    if (border) {
        box(frill_window, 0, 0);
    }

    // If we have a message, draw it.
    if (message.size()) {
        if (border) {
            mvwprintw(frill_window, 1, 1, "%s", message.c_str());
        } else {
            mvwprintw(frill_window, 0, 0, "%s", message.c_str());
        }
    }

    refresh();

    post_menu(menu);
    wrefresh(frill_window);

    bool done = false;
    bool hit_escape = false;
    while (!done) {
        int ch = wgetch(frill_window);
        switch (ch) {
        case 27:  // escape
            hit_escape = true;
            done = true;
            break;
        case KEY_DOWN:
        case 'j':
            menu_driver(menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
        case 'k':
            menu_driver(menu, REQ_UP_ITEM);
            break;
        case '\r':
            done = true;
            break;
        }
    }

    // Get the selected item:
    string answer = choices[item_index(current_item(menu))];
    if (hit_escape) {
        answer = "";
    }

    unpost_menu(menu);
    for (int i = 0; i < choices.size(); ++i) {
        free_item(items[i]);
    }
    free_menu(menu);
    delwin(text_window);
    delwin(frill_window);
    free(items);

    return answer;
}

bool ListChooser::GetYesNo(const string& message, bool default_yes) {
    vector<string> choices;
    if (default_yes) {
        choices.push_back("Yes");
        choices.push_back("No");
    } else {
        choices.push_back("No");
        choices.push_back("Yes");
    }

    return ListChooser::GetChoiceWithOptions(choices,
            message,
            COLOR_PAIR(0) | A_REVERSE,
            COLOR_PAIR(0),
            -1,
            -1,
            true,
            ">") == "Yes";
}

string ListChooser::GetChoice(const vector<string>& choices) {
    return GetChoiceWithOptions(choices,
                                "",
                                COLOR_PAIR(0) | A_REVERSE,
                                COLOR_PAIR(0),
                                -1,
                                -1,
                                true,
                                " ");
}

string ListChooser::GetMappedChoice(const map<string, string>& mappedChoices) {
    vector<string> displayedChoices;
    for (map<string, string>::const_iterator it = mappedChoices.begin(); it != mappedChoices.end(); ++it) {
        displayedChoices.push_back(it->first);
    }
    string displayedSelection = GetChoiceWithOptions(displayedChoices,
                                "",
                                COLOR_PAIR(0) | A_REVERSE,
                                COLOR_PAIR(0),
                                -1,
                                -1,
                                true,
                                " ");
    string answer = "";
    if(!displayedSelection.empty()) {
        map<string, string>::const_iterator item = mappedChoices.find(displayedSelection);
        answer = item->second;
    }
    return answer;
}

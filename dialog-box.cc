#include "dialog-box.h"

string DialogBox::RunMultiLine(const string& title, const string& default_text,
                               int width, int height) {
  // Create the text field
  FIELD* field = new_field(height, width, 0, 0, 0, 0);

  // Set its options
  set_field_back(field, A_UNDERLINE);
  field_opts_off(field, O_STATIC);
  if (!default_text.empty()) {
    set_field_buffer(field, 0, default_text.c_str());
  }

  // Make the usual list of fields
  FIELD* fields[] = {field, NULL};

  // Finally make our form
  FORM* form = new_form(fields);

  // Create a window big enough to hold our form.  Then make it slightly
  // bigger to allow space for borders and the title text.
  // Get the min size for the form:
  int rows, cols;
  scale_form(form, &rows, &cols);

  // Figure out how many lines of text we'll have for the label:
  int label_rows = 1;  // ceil(title.size() / cols);

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
  WINDOW* form_win =
      newwin(form_win_rows, form_win_cols, info.height / 2 - form_win_rows / 2,
             info.width / 2 - form_win_cols / 2);

  // Enable the arrow keys in our new window
  keypad(form_win, true);

  // Set the form's parent window to be this new window
  set_form_win(form, form_win);

  // Make the subwindow for the form itself
  WINDOW* subwindow = derwin(form_win, rows, cols, 2 + label_rows, 3);
  set_form_sub(form, subwindow);

  // Draw a box around our form's window
  // wborder(form_win, '|', '|', '-', '-', '+', '+', '+', '+');
  box(form_win, 0, 0);

  // Draw the label text
  CursesUtils::print_in_middle(form_win, 1, 0, form_win_cols, title.c_str(),
                               COLOR_PAIR(1));

  // Show the form window
  post_form(form);
  wrefresh(form_win);

  bool done = false;
  bool hit_escape = false;
  while (!done) {
    int ch = wgetch(form_win);
    switch (ch) {
      case 27:  // escape
        hit_escape = true;
        done = true;
        break;
      case KEY_LEFT:  // left arrow
        form_driver(form, REQ_PREV_CHAR);
        break;
      case KEY_RIGHT:  // right arrow
        form_driver(form, REQ_NEXT_CHAR);
        break;
      case KEY_UP:  // up arrow
        form_driver(form, REQ_PREV_LINE);
        break;
      case KEY_DOWN:  // dn arrow
        form_driver(form, REQ_NEXT_LINE);
        break;
      case 263:  // delete
      case 127:  // also delete
        form_driver(form, REQ_DEL_PREV);
        break;
      case '\r':
        done = true;
        // form_driver(form, REQ_NEW_LINE);
        break;
      default:
        form_driver(form, ch);
    }
  }

  // Without calling this the output doesn't actually get put in the buffer.
  form_driver(form, REQ_VALIDATION);

  // Get whatever they wrote:
  string answer(field_buffer(field, 0));

  // trim trailing whitespace
  int notwhite = answer.find_last_not_of(" \t\n");
  answer.erase(notwhite + 1);
  StrUtils::trim_multiple_spaces(answer);

  // Free up our memory
  unpost_form(form);
  free_form(form);
  free_field(field);
  delwin(subwindow);
  delwin(form_win);

  if (hit_escape) {
    // Clear out whatever was in the box if they hit escape.
    answer = "";
  }
  return answer;
}

string DialogBox::RunCentered(const string& title, const string& default_text) {
  return DialogBox::RunMultiLine(title, default_text, title.size(), 1);
}

string DialogBox::RunCenteredWithWidth(const string& title,
                                       const string& default_text,
                                       const int width) {
  return DialogBox::RunMultiLine(title, default_text, width, 1);
}

#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include <ncursesw/form.h>
#include "dialog-box.h"
#include <cassert>


#undef CTRL
#define CTRL(x)	((x) & 0x1f)
#define ESCAPE		CTRL('[')

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
  FIELD* fields[] = {field, nullptr};

  // Finally make our form
  FORM* form = new_form(fields);

  // Create a window big enough to hold our form.  Then make it slightly
  // bigger to allow space for borders and the title text.
  // Get the min size for the form:
  int cols;
  int rows;
  scale_form(form, &rows, &cols);

  // Figure out how many lines of text we'll have for the label:
  int label_rows = 1;

  // Account for the two side borders, plus two spaces on either side of the
  // text entry box:
  int form_win_cols = cols + 2 + 4;

  // Account for the top and bottom borders, any label lines and the divider
  // between the label region and the text region.
  int form_win_rows = rows + 2 + label_rows + 1;

  // Get the dimensions of the main screen, so we can put this new window in the
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
  CursesUtils::print_in_middle(form_win, 1, 0, form_win_cols, title,
                               COLOR_PAIR(1));

  // Show the form window
  post_form(form);
  wrefresh(form_win);

  bool done = false;
  bool hit_escape = false;
    while (!done) {

        wint_t c2;
        int ret = wget_wch(form_win, &c2);


        switch (ret) {
            case KEY_CODE_YES:
                switch (c2){ //we have a key code, so deal with that (not a wide char)
                    case KEY_LEFT:
                        form_driver_w(form, KEY_CODE_YES, REQ_PREV_CHAR);
                        break;
                    case KEY_RIGHT:  // right arrow
                        form_driver_w(form, KEY_CODE_YES, REQ_NEXT_CHAR);
                        break;
                    case KEY_UP:  // up arrow
                        form_driver_w(form, KEY_CODE_YES, REQ_PREV_LINE);
                        break;
                    case KEY_DOWN:  // dn arrow
                        form_driver_w(form, KEY_CODE_YES, REQ_NEXT_LINE);
                        break;
                    case KEY_DC:
                    case KEY_DL:
                        form_driver_w(form, KEY_CODE_YES, REQ_DEL_CHAR);
                        break;
                }
                break;
            case OK:
                switch (c2){
                    case ESCAPE:
                        hit_escape = true;
                        done = true;
                        break;
                    case '\n':
                    case '\r':
                        done = true;
                        //form_driver_w(form, OK, REQ_END_LINE);
                        break;
                    case 127:
                        form_driver_w(form, KEY_CODE_YES, REQ_DEL_PREV); // KEY_CODE_YES is strictly needed!
                        break;
                    default:
                        form_driver_w(form, OK, (wchar_t) c2);
                }
                break;
            default:
                break;
        }
    }

  // Without calling this the output doesn't actually get put in the buffer.
  int rc = form_driver_w(form, KEY_CODE_YES, REQ_VALIDATION);
  assert(E_OK==rc);
  // Get whatever they wrote:
  string answer(field_buffer(fields[0], 0));


  // trim trailing whitespace
  int notwhite = (int)answer.find_last_not_of(" \t\n");
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
  return DialogBox::RunMultiLine(title, default_text, (int)title.size(), 1);
}

string DialogBox::RunCenteredWithWidth(const string& title,
                                       const string& default_text,
                                       const int width) {
  return DialogBox::RunMultiLine(title, default_text, width, 1);
}

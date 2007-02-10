#include "dialog-box.h"

#define max(a,b) ((a) > (b) ? (a) : (b))

static void print_in_middle(WINDOW *win,
    int starty,
    int startx,
    int width,
    const char *string,
    chtype color) {
  int length, x, y;
  float temp;

  if(win == NULL)
    win = stdscr;
  getyx(win, y, x);
  if(startx != 0)
    x = startx;
  if(starty != 0)
    y = starty;
  if(width == 0)
    width = 80;

  length = strlen(string);
  temp = (width - length)/ 2;
  x = startx + (int)temp;
  wattron(win, color | A_BOLD);
  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color | A_BOLD);

  // Draw a horizontal line
  wmove(win, 2, 0);
  waddch(win, '+');
  for (int i = 1; i < width - 1; ++i) {
    waddch(win, '-');
  }
  waddch(win, '+');
  refresh();
}

string DialogBox::RunMultiLine(const string& title,
    const string& default_text,
    int height) {
  // Create the text field
  FIELD* field = new_field(height, title.size(), 0, 0, 0, 0);

  // Set its options
  set_field_back(field, A_UNDERLINE);
  field_opts_off(field, O_STATIC);
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
  window_info info = get_window_info(stdscr);

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
  set_form_sub(form, derwin(form_win, rows, cols, 2 + label_rows, 3));

  // Draw a box around our form's window
  wborder(form_win, '|', '|', '-', '-', '+', '+', '+', '+');

  // Draw the label text
  print_in_middle(form_win, 1, 0, form_win_cols, title.c_str(), COLOR_PAIR(1));

  // Show the form window
  post_form(form);
  wrefresh(form_win);

  int ch;
  bool done = false;
  bool hit_escape = false;
  while (!done) {
    ch = wgetch(form_win);
    switch (ch) {
      case 27:  // escape
        hit_escape = true;
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
      case 263: // delete
        form_driver(form,REQ_DEL_PREV);
        break;
      case '\r':
        done = true;
        //form_driver(form, REQ_NEW_LINE);
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

  // Free up our memory
  unpost_form(form);
  free_form(form);
  free_field(field);
  delwin(form_win);

  return answer;
}

string DialogBox::RunCentered(const string& title, const string& default_text) {
  return DialogBox::RunMultiLine(title, default_text, 1);
}

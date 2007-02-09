#include "project.h"
#include "form.h"


Project::Project(string name)
  : name_(name) {
}

Project::~Project() {

}

static void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color) { int length, x, y;
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
  wattron(win, color);
  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color);
  refresh();
}

Project* Project::NewProject() {
  // Create the form that will handle this.
  // First create the project name field
  FIELD* pname_field = new_field(1, /* height */
      40,/* width */
      1, /* top */
      1, /* left */
      0, /* offscreen rows */
      0  /* extra working buffers */);
  set_field_back(pname_field, A_UNDERLINE);
  field_opts_off(pname_field, O_STATIC);
  set_field_buffer(pname_field, 0, "Example Project");

  // Create a list of our fields.
  FIELD* fields[] = { pname_field, NULL };

  // Create our form.
  FORM* form = new_form(fields);

  // Make a new window that will hold the form.
  int rows, cols;
  scale_form(form, &rows, &cols);

  WINDOW* form_win = newwin(rows + 3, cols + 5, 20, 20);
  keypad(form_win, true);

  set_form_win(form, form_win);
  set_form_sub(form, derwin(form_win, rows, cols, 2, 2));

  // Draw a box around the panel
  box(form_sub(form), 0, 0);
  box(form_win, 0, 0);
  print_in_middle(form_win, 1, 0, cols + 4, "Please Enter a Project Name", COLOR_PAIR(1));
  
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
    mvprintw(LINES - 1, 0, "%d", ch);
    refresh();
  }

  // Without calling this the output doesn't actually get put in the buffer.
  form_driver(form, REQ_VALIDATION);

  // Potentially create a new project.
  Project* to_return = NULL;
  if (!hit_escape) {
    to_return = new Project(field_buffer(pname_field, 0));
  }

  // Free up our fields and form
  unpost_form(form);
  free_form(form);
  free_field(pname_field);
  delwin(form_win);
  
  return to_return;
}

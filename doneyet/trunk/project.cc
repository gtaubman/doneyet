#include "project.h"
#include "dialog-box.h"


Project::Project(string name)
  : name_(name) {
}

Project::~Project() {

}

static void print_in_middle(WINDOW *win,
    int starty,
    int startx,
    int width,
    char *string,
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
  wattron(win, color);
  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color);

  // Draw a horizontal line
  wmove(win, 2, 0);
  waddch(win, '+');
  for (int i = 1; i < width - 1; ++i) {
    waddch(win, '-');
  }
  waddch(win, '+');
  refresh();
}

Project* Project::NewProject() {
  // Ask for a project name
  string answer = DialogBox::RunCentered(
      "Please Enter a Project Name",
      "Default Project");
  Project* p = NULL;
  if (!answer.empty()) {
    p = new Project(answer);
  }
  return p;
}

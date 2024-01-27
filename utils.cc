#include "utils.h"

#include <string.h>

// Returns the origin and dimensions of an ncurses window.
window_info CursesUtils::get_window_info(WINDOW* win) {
  // The struct to return;
  window_info info;

  // Get the origin of the window
  int x, y;
  getyx(win, y, x);
  info.origin_x = x;
  info.origin_y = y;

  // Get the size of the window
  getmaxyx(win, y, x);
  info.width = x;
  info.height = y;

  return info;
}

void CursesUtils::print_in_middle(WINDOW* win, int starty, int startx,
                                  int width, const std::string str, chtype color) {
  int length;
  int x;
  int y;
  float temp;

  if (win == nullptr) win = stdscr;
  getyx(win, y, x);
  if (starty != 0) y = starty;
  if (width == 0) width = 80;

  length = str.length();
  temp = float(width - length) / 2;
  x = startx + (int)temp;
  wattron(win, color | A_BOLD);
  mvwprintw(win, y, x, "%s", str.c_str());
  wattroff(win, color | A_BOLD);

  // Draw a horizontal line
  wmove(win, 2, 0);
  waddch(win, ACS_LTEE);
  for (int i = 1; i < width - 1; ++i) {
    waddch(win, ACS_HLINE);
  }
  waddch(win, ACS_RTEE);
}

int CursesUtils::winheight(WINDOW* win) {
  int w, h;
  getmaxyx(win, h, w);
  return h;
}

int CursesUtils::winwidth(WINDOW* win) {
  int w;
  int __attribute__((unused)) h;
  getmaxyx(win, h, w);
  return w;
}

int StrUtils::chars_to_whitespace(const string& str, int i) {
  int p = str.find(" ", i);
  return p == string::npos ? str.size() - i : p - i;
}

void StrUtils::trim_multiple_spaces(string& str) {
  int space = 0;
  while (true) {
    space = str.find_first_of(" ", space);
    if (space == string::npos) {
      return;
    }
    int next_char = str.find_first_not_of(" ", space);
    ++space;
    str.erase(space, next_char - space);
    ++space;
  }
}

void StrUtils::SplitStringUsing(const string splitter, const string str,
                                vector<string>* vec) {
  if (!splitter.size()) {
    return;
  }

  int start_pos = 0;
  size_t find_pos;
  while ((find_pos = str.find(splitter, start_pos)) != string::npos) {
    vec->push_back(str.substr(start_pos, find_pos - start_pos));
    start_pos = find_pos + splitter.size();
  }
  if (start_pos != str.size()) {
    vec->push_back(str.substr(start_pos, find_pos));
  }
}

int StrUtils::HeightOfTextInWidth(int width, const string& text,
                                  int non_first_line_indent) {
  int lines_used = 1;
  int curx = 0;
  for (int i = 0; i < text.size(); ++i) {
    int chars_to_ws = chars_to_whitespace(text, i);
    if (curx + chars_to_ws > width) {
      ++lines_used;
      curx = non_first_line_indent;
    }
    ++curx;
  }
  return lines_used;
}

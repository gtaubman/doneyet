#ifndef UTILS_H_
#define UTILS_H_

#include <ncurses.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

typedef struct {
    int origin_x, origin_y;
    int width, height;
} window_info;

class CursesUtils {
  public:
    // Returns the origin and dimensions of an ncurses window.
    static window_info get_window_info(WINDOW* win);
    static void print_in_middle(WINDOW *win,
                                int starty,
                                int startx,
                                int width,
                                const char *string,
                                chtype color);

    static int winheight() {
        return winheight(stdscr);
    }
    static int winwidth() {
        return winwidth(stdscr);
    }
    static int winheight(WINDOW* win);
    static int winwidth(WINDOW* win);
};

class StrUtils {
  public:
    static int chars_to_whitespace(const string& str, int i);
    static void trim_multiple_spaces(string& str);
    static void SplitStringUsing(const string splitter,
                                 const string str,
                                 vector<string>* vec);
    static int HeightOfTextInWidth(int width, const string& text,
                                   int non_first_line_indent);
};

#endif  // UTILS_H_

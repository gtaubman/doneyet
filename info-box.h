#ifndef INFO_BOX_H_
#define INFO_BOX_H_

#include <math.h>
#include <ncurses/form.h>
#include <ncurses/ncurses.h>

#include <string>

#include "utils.h"

using std::string;

class InfoBox {
 public:
  static void ShowMultiLine(const string& title, const string& default_text,
                            int width, int height);
  static void ShowFullScreen(const string& title, const string& default_text,
                             int width, int height);
};

#endif  // INFO_BOX_H_

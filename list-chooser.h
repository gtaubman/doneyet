#ifndef LIST_CHOOSER_H_
#define LIST_CHOOSER_H_

#include <map>
#include <string>
#include <vector>

#include "curses-menu.h"

using std::map;
using std::string;
using std::vector;

class ListChooser {
 public:
  static string GetChoiceWithOptions(const vector<string>& choices,
                                     const string& message, int fg_color,
                                     int bg_color, int xloc, int yloc,
                                     bool border, string mark);

  static bool GetYesNo(const string& message, bool default_yes);

  static string GetChoice(const vector<string>& choices);

  static string GetMappedChoice(const map<string, string>& mappedChoices);
};

#endif  // LIST_CHOOSER_H_

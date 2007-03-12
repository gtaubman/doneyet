#ifndef __DIALOG_BOX__
#define __DIALOG_BOX__

#include <ncurses.h>
#include <form.h>
#include <math.h>
#include <string>

#include "utils.h"

using std::string;

class DialogBox {
 public:
   static string RunMultiLine(
       const string& title,
       const string& default_text,
       int width,
       int height);

  static string RunCentered(
      const string& title,
      const string& default_text);

  static string RunCenteredWithWidth(
      const string& title,
      const string& default_text,
      const int width);
};

#endif

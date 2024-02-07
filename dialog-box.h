#ifndef DIALOG_BOX_H_
#define DIALOG_BOX_H_

#include <cmath>
#include <string>
#include "utils.h"

using std::string;

class DialogBox {
 public:
  static string RunMultiLine(const string& title, const string& default_text,
                             int width, int height);

  static string RunCentered(const string& title, const string& default_text);

  static string RunCenteredWithWidth(const string& title,
                                     const string& default_text,
                                     int width);
};

#endif  // DIALOG_BOX_H_

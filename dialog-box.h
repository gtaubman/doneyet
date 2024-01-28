#ifndef DIALOG_BOX_H_
#define DIALOG_BOX_H_

#define _XOPEN_SOURCE_EXTENDED
#include <math.h>
#include <ncursesw/form.h>
#include <ncursesw/ncurses.h>

#include <string>

#include "utils.h"

using std::wstring;

class DialogBox {
 public:
  static wstring RunMultiLine(const wstring& title, const wstring& default_text,
                             int width, int height);

  static wstring RunCentered(const wstring& title, const wstring& default_text);

  static wstring RunCenteredWithWidth(const wstring& title,
                                     const wstring& default_text,
                                     const int width);
};

#endif  // DIALOG_BOX_H_

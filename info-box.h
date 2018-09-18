#ifndef __INFO_BOX__
#define __INFO_BOX__

#include <ncurses.h>
#include <form.h>
#include <math.h>
#include <string>

#include "utils.h"

using std::string;

class InfoBox {
 public:
   static void ShowMultiLine(
       const string& title,
       const string& default_text,
       int width,
       int height);
};

#endif

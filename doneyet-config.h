#ifndef _DONEYET_CONFIG_H_
#define _DONEYET_CONFIG_H_

#include <map>
#include <string>

class DoneyetConfig {
 public:
  bool Parse();

  // Color related configuration.
  short ForegroundColor() { return foreground_color_; }
  short BackgroundColor() { return background_color_; }

 private:
  std::map<std::string, std::map<std::string, std::string> > config_;

  bool ParseColors();
  short foreground_color_;
  short background_color_;
};

#endif

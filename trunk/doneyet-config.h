#ifndef _DONEYET_CONFIG_H_
#define _DONEYET_CONFIG_H_

#include <map>
#include <string>

using std::map;
using std::string;

class DoneyetConfig {
 public:
  bool Parse();

  // Color related configuration.
  short ForegroundColor() { return foreground_color_; }
  short BackgroundColor() { return background_color_; }

 private:
  map<string, map<string, string> > config_;

  // Attempts to convert config[color_name] into a color, and set var_to_set to
  // that color.  If this fails it prints an error and return false.
  bool ParseColor(map<string, string>& config,
                  const string& color_name,
                  short* var_to_set);

  bool ParseGeneralOptions();
  short foreground_color_;
  short background_color_;

  bool ParseTaskOptions();
  short unstarted_task_color_;
  short in_progress_task_color_;
  short paused_task_color_;
  short finished_task_color_;

  bool ParseMenuOptions();
  short unselected_menu_foreground_color_;
  short unselected_menu_background_color_;
  short selected_menu_foreground_color_;
  short selected_menu_background_color_;
};

#endif

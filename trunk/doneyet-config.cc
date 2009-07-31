#include <cctype>
#include <ncurses.h>
#include "doneyet-config.h"
#include "file-manager.h"
#include "config-parser.h"

static const short kColorError = -2;

static const char* kGeneralSection = "GENERAL";
static const char* kForegroundColor = "foreground_color";
static const char* kBackgroundColor = "background_color";
static const char* kHeaderTextColor = "header_text_color";

static const char* kTasksSection = "TASKS";
static const char* kUnstartedTaskColor = "unstarted_color";
static const char* kInProgressColor = "in_progress_color";
static const char* kPausedColor = "paused_color";
static const char* kFinishedColor = "finished_color";

static const char* kMenusSection = "MENUS";
static const char* kUnselectedMenuItemForegroundColor =
    "unselected_foreground_color";
static const char* kUnselectedMenuItemBackgroundColor =
    "unselected_background_color";
static const char* kSelectedMenuItemForegroundColor =
    "selected_foreground_color";
static const char* kSelectedMenuItemBackgroundColor =
    "selected_background_color";

bool DoneyetConfig::Parse() {
  FileManager* file_manager = FileManager::DefaultFileManager();

  // Load our defaults.
  map<string, string>& general = config_[kGeneralSection];
  general[kForegroundColor] = "white";
  general[kBackgroundColor] = "black";
  general[kHeaderTextColor] = "red";

  map<string, string>& tasks = config_[kTasksSection];
  tasks[kUnstartedTaskColor] = "terminal";
  tasks[kInProgressColor] = "green";
  tasks[kPausedColor] = "red";
  tasks[kFinishedColor] = "blue";

  map<string, string>& menus = config_[kMenusSection];
  menus[kUnselectedMenuItemForegroundColor] = "yellow";
  menus[kUnselectedMenuItemBackgroundColor] = "blue";
  menus[kSelectedMenuItemForegroundColor] = "yellow";
  menus[kSelectedMenuItemBackgroundColor] = "blue";

  if (file_manager->ConfigFilePath().length() &&
      !ConfigParser::ParseConfig(file_manager->ConfigFilePath(),
                                 &config_)) {
    return false;
  }

  return ParseGeneralOptions() &&
      ParseTaskOptions() &&
      ParseMenuOptions();
}

short DoneyetConfig::ForegroundColor() {
  return foreground_color_;
}

short DoneyetConfig::BackgroundColor() {
  return background_color_;
}

short DoneyetConfig::HeaderTextColor() {
  return header_text_color_;
}

short DoneyetConfig::UnstartedTaskColor() {
  return unstarted_task_color_;
}

short DoneyetConfig::InProgressTaskColor() {
  return in_progress_task_color_;
}

short DoneyetConfig::PausedTaskColor() {
  return paused_task_color_;
}

short DoneyetConfig::FinishedTaskColor() {
  return finished_task_color_;
}

short DoneyetConfig::UnselectedMenuForegroundColor() {
  return unselected_menu_foreground_color_;
}

short DoneyetConfig::UnselectedMenuBackgroundColor() {
  return unselected_menu_background_color_;
}

short DoneyetConfig::SelectedMenuForegroundColor() {
  return selected_menu_foreground_color_;
}

short DoneyetConfig::SelectedMenuBackgroundColor() {
  return selected_menu_background_color_;
}

static short ColorForString(string s) {
  // First lowercase.
  for (int i = 0; i < s.length(); ++i) {
    s[i] = std::tolower(s[i]);
  }

  if (s == "black") {
    return COLOR_BLACK;
  } else if (s == "red") {
    return COLOR_RED;
  } else if (s == "green") {
    return COLOR_GREEN;
  } else if (s == "yellow") {
    return COLOR_YELLOW;
  } else if (s == "blue") {
    return COLOR_BLUE;
  } else if (s == "magenta") {
    return COLOR_MAGENTA;
  } else if (s == "cyan") {
    return COLOR_CYAN;
  } else if (s == "white") {
    return COLOR_WHITE;
  } else if (s == "terminal") {
    return -1;
  }

  return kColorError;
}

bool DoneyetConfig::ParseColor(map<string, string>& config,
                               const string& color_name,
                               short* var_to_set) {
  *var_to_set = ColorForString(config[color_name]);
  if (*var_to_set == kColorError) {
    fprintf(stderr,
            "'%s' is not a valid color for config option %s.\n",
            config[color_name].c_str(),
            color_name.c_str());
    return false;
  }

  return true;
}

bool DoneyetConfig::ParseGeneralOptions() {
  // Get the general section.
  map<string, string>& general = config_[kGeneralSection]; 

  return ParseColor(general, kForegroundColor, &foreground_color_) &&
      ParseColor(general, kBackgroundColor, &background_color_) &&
      ParseColor(general, kHeaderTextColor, &header_text_color_);
}

bool DoneyetConfig::ParseTaskOptions() {
  map<string, string>& task = config_[kTasksSection];

  return ParseColor(task, kUnstartedTaskColor, &unstarted_task_color_) &&
      ParseColor(task, kInProgressColor, &in_progress_task_color_) &&
      ParseColor(task, kPausedColor, &paused_task_color_) &&
      ParseColor(task, kFinishedColor, &finished_task_color_);
}

bool DoneyetConfig::ParseMenuOptions() {
  map<string, string>& menu = config_[kMenusSection];

  return ParseColor(menu, kUnselectedMenuItemForegroundColor,
                    &unselected_menu_foreground_color_) &&
      ParseColor(menu, kUnselectedMenuItemBackgroundColor,
                 &unselected_menu_background_color_) &&
      ParseColor(menu, kSelectedMenuItemForegroundColor,
                 &selected_menu_foreground_color_) &&
      ParseColor(menu, kSelectedMenuItemBackgroundColor,
                 &selected_menu_background_color_);
}

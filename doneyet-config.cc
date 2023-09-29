#include "doneyet-config.h"

#include <ncursesw/ncurses.h>

#include <cctype>

#include "config-parser.h"
#include "file-manager.h"

static const short kColorError = -2;

static const char* const kGeneralSection = "GENERAL";
static const char* const kForegroundColor = "foreground_color";
static const char* const kBackgroundColor = "background_color";
static const char* const kHeaderTextColor = "header_text_color";

static const char* const kTasksSection = "TASKS";
static const char* const kUnstartedTaskColor = "unstarted_color";
static const char* const kInProgressColor = "in_progress_color";
static const char* const kPausedColor = "paused_color";
static const char* const kFinishedColor = "finished_color";
static const char* const kPromptOnDeleteTask = "prompt_on_delete";

static const char* const kMenusSection = "MENUS";
static const char* const kMenubarForegroundColor = "bar_foreground_color";
static const char* const kMenubarBackgroundColor = "bar_background_color";
static const char* const kUnselectedMenuItemForegroundColor =
    "unselected_item_foreground_color";
static const char* const kUnselectedMenuItemBackgroundColor =
    "unselected_item_background_color";
static const char* const kSelectedMenuItemForegroundColor =
    "selected_item_foreground_color";
static const char* const kSelectedMenuItemBackgroundColor =
    "selected_item_background_color";

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
  tasks[kPromptOnDeleteTask] = "true";

  map<string, string>& menus = config_[kMenusSection];
  menus[kMenubarForegroundColor] = "black";
  menus[kMenubarBackgroundColor] = "yellow";
  menus[kUnselectedMenuItemForegroundColor] = "yellow";
  menus[kUnselectedMenuItemBackgroundColor] = "blue";
  menus[kSelectedMenuItemForegroundColor] = "yellow";
  menus[kSelectedMenuItemBackgroundColor] = "blue";

  if (file_manager->ConfigFilePath().length() &&
      !ConfigParser::ParseConfig(file_manager->ConfigFilePath(), &config_)) {
    return false;
  }

  return ParseGeneralOptions() && ParseTaskOptions() && ParseMenuOptions();
}

short DoneyetConfig::ForegroundColor() const { return foreground_color_; }

short DoneyetConfig::BackgroundColor() const { return background_color_; }

short DoneyetConfig::HeaderTextColor() const { return header_text_color_; }

short DoneyetConfig::UnstartedTaskColor() const {
  return unstarted_task_color_;
}

short DoneyetConfig::InProgressTaskColor() const {
  return in_progress_task_color_;
}

short DoneyetConfig::PausedTaskColor() const { return paused_task_color_; }

short DoneyetConfig::FinishedTaskColor() const { return finished_task_color_; }

bool DoneyetConfig::PromptOnDeleteTask() const {
  return prompt_on_delete_task_;
}

short DoneyetConfig::MenubarForegroundColor() const {
  return menubar_foreground_color_;
}

short DoneyetConfig::MenubarBackgroundColor() const {
  return menubar_background_color_;
}

short DoneyetConfig::UnselectedMenuForegroundColor() const {
  return unselected_menu_foreground_color_;
}

short DoneyetConfig::UnselectedMenuBackgroundColor() const {
  return unselected_menu_background_color_;
}

short DoneyetConfig::SelectedMenuForegroundColor() const {
  return selected_menu_foreground_color_;
}

short DoneyetConfig::SelectedMenuBackgroundColor() const {
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
                               short* var_to_set) const {
  *var_to_set = ColorForString(config[color_name]);
  if (*var_to_set == kColorError) {
    fprintf(stderr, "'%s' is not a valid color for config option %s.\n",
            config[color_name].c_str(), color_name.c_str());
    return false;
  }

  return true;
}

bool DoneyetConfig::ParseBool(map<string, string>& config,
                              const string& to_parse, bool* value) const {
  const string& param = config[to_parse];
  if (param == "true" || param == "yes") {
    *value = true;
  } else if (param == "false" || param == "no") {
    *value = false;
  } else {
    fprintf(stderr, "'%s' is not a valid boolean option.", param.c_str());
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
         ParseColor(task, kFinishedColor, &finished_task_color_) &&
         ParseBool(task, kPromptOnDeleteTask, &prompt_on_delete_task_);
}

bool DoneyetConfig::ParseMenuOptions() {
  map<string, string>& menu = config_[kMenusSection];

  return ParseColor(menu, kMenubarForegroundColor,
                    &menubar_foreground_color_) &&
         ParseColor(menu, kMenubarBackgroundColor,
                    &menubar_background_color_) &&
         ParseColor(menu, kUnselectedMenuItemForegroundColor,
                    &unselected_menu_foreground_color_) &&
         ParseColor(menu, kUnselectedMenuItemBackgroundColor,
                    &unselected_menu_background_color_) &&
         ParseColor(menu, kSelectedMenuItemForegroundColor,
                    &selected_menu_foreground_color_) &&
         ParseColor(menu, kSelectedMenuItemBackgroundColor,
                    &selected_menu_background_color_);
}

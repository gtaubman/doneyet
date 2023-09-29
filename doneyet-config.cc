#include "doneyet-config.h"

#include <ncursesw/ncurses.h>

#include <cctype>

#include "config-parser.h"
#include "file-manager.h"

static const short kColorError = -2;

static const char* const kGeneralSection = "GENERAL";
static const char* const kForegroundColor = "foreground_color";
static const char* const kBackgroundColor = "background_color";

static const char* const kTasksSection = "TASKS";
static const char* const kPromptOnDeleteTask = "prompt_on_delete";

bool DoneyetConfig::Parse() {
  FileManager* file_manager = FileManager::DefaultFileManager();

  // Load our defaults.
  map<string, string>& general = config_[kGeneralSection];
  general[kForegroundColor] = "white";
  general[kBackgroundColor] = "black";

  map<string, string>& tasks = config_[kTasksSection];
  tasks[kPromptOnDeleteTask] = "true";

  if (file_manager->ConfigFilePath().length() &&
      !ConfigParser::ParseConfig(file_manager->ConfigFilePath(), &config_)) {
    return false;
  }

  return ParseGeneralOptions();
}

short DoneyetConfig::ForegroundColor() const { return foreground_color_; }

short DoneyetConfig::BackgroundColor() const { return background_color_; }

bool DoneyetConfig::PromptOnDeleteTask() const {
  return prompt_on_delete_task_;
}

static short ColorForString(string s) {
  // First lowercase.
  for (int i = 0; i < s.length(); ++i) {
    s[i] = (char) std::tolower(s[i]);
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
    fprintf(stderr, "'%s' is not a valid color for config option %s.\n",
            config[color_name].c_str(), color_name.c_str());
    return false;
  }

  return true;
}

bool DoneyetConfig::ParseBool(map<string, string>& config,
                              const string& to_parse, bool* value) {
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
  map<string, string>& task = config_[kTasksSection];

  return ParseColor(general, kForegroundColor, &foreground_color_) &&
         ParseColor(general, kBackgroundColor, &background_color_) &&
         ParseBool(task, kPromptOnDeleteTask, &prompt_on_delete_task_);
}

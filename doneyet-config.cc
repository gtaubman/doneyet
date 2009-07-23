#include <cctype>
#include <ncurses.h>
#include "doneyet-config.h"
#include "file-manager.h"
#include "config-parser.h"

static const short kColorError = -2;

static const char* kGeneralSection = "GENERAL";
static const char* kForegroundColor = "foreground_color";
static const char* kBackgroundColor = "background_color";

static const char* kTasksSection = "TASKS";
static const char* kUnstartedTaskColor = "unstarted_color";
static const char* kInProgressColor = "in_progress_color";
static const char* kPausedColor = "paused_color";
static const char* kFinishedColor = "finished_color";

bool DoneyetConfig::Parse() {
  FileManager* file_manager = FileManager::DefaultFileManager();

  // Load our defaults.
  map<string, string>& general = config_[kGeneralSection];
  general[kForegroundColor] = "white";
  general[kBackgroundColor] = "black";

  map<string, string>& tasks = config_[kTasksSection];
  tasks[kUnstartedTaskColor] = "default";
  tasks[kInProgressColor] = "green";
  tasks[kPausedColor] = "red";
  tasks[kFinishedColor] = "blue";

  if (file_manager->ConfigFilePath().length() &&
      !ConfigParser::ParseConfig(file_manager->ConfigFilePath(),
                                 &config_)) {
    return false;
  }

  if (!ParseColors()) {
    return false;
  }

  return true;
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

bool DoneyetConfig::ParseColors() {
  // Get the general section.
  map<string, string>& general = config_[kGeneralSection]; 

  foreground_color_ = ColorForString(general[kForegroundColor]);
  if (foreground_color_ == kColorError) {
    fprintf(stderr, "Unable to load foreground color from config file.\n");
    return false;
  }

  background_color_ = ColorForString(general[kBackgroundColor]);
  if (background_color_ == kColorError) {
    fprintf(stderr, "Unable to load background color from config file.\n");
    return false;
  }

  fprintf(stderr, "Successfully loaded colors.\n");
  return true;
}

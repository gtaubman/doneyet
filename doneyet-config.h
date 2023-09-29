#ifndef DONEYET_CONFIG_H_
#define DONEYET_CONFIG_H_

// This class is used to wrap the parsing and validation of all configuration
// options.  These options can be accessed directly, or (in the case of colors,
// for example) can be further wrapped before handing off to the rest of the
// system.

#include <map>
#include <string>

using std::map;
using std::string;

class DoneyetConfig;

static DoneyetConfig* global_config = nullptr;

class DoneyetConfig {
 public:
  static DoneyetConfig* GlobalConfig() {
    if (global_config == nullptr) {
      global_config = new DoneyetConfig();
      if (!global_config->Parse()) {
        delete global_config;
        global_config = nullptr;
        return nullptr;
      }
    }
    return global_config;
  }

  bool Parse();

  // General related configuration.
  short ForegroundColor() const;
  short BackgroundColor() const;
  bool PromptOnDeleteTask() const;

 private:
  DoneyetConfig() {}

  map<string, map<string, string> > config_;

  // Attempts to convert config[color_name] into a color, and set var_to_set to
  // that color.  If this fails it prints an error and return false.
  static bool ParseColor(map<string, string>& config, const string& color_name,
                  short* var_to_set);
  static bool ParseBool(map<string, string>& config, const string& to_parse,
                 bool* value);

  bool ParseGeneralOptions();
  short foreground_color_;
  short background_color_;
  bool prompt_on_delete_task_;

};

#endif  // DONEYET_CONFIG_H_

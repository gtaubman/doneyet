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

static DoneyetConfig* global_config = NULL;

class DoneyetConfig {
  public:
    static DoneyetConfig* GlobalConfig() {
        if (global_config == NULL) {
            global_config = new DoneyetConfig();
            if (!global_config->Parse()) {
                delete global_config;
                global_config = NULL;
                return NULL;
            }
        }
        return global_config;
    }

    bool Parse();

    // General related configuration.
    short ForegroundColor();
    short BackgroundColor();
    short HeaderTextColor();

    // Task related configuration.
    short UnstartedTaskColor();
    short InProgressTaskColor();
    short PausedTaskColor();
    short FinishedTaskColor();
    bool PromptOnDeleteTask();

    // Menu related configuration.
    short MenubarForegroundColor();
    short MenubarBackgroundColor();
    short UnselectedMenuForegroundColor();
    short UnselectedMenuBackgroundColor();
    short SelectedMenuForegroundColor();
    short SelectedMenuBackgroundColor();

  private:
    DoneyetConfig() { }

    map<string, map<string, string> > config_;

    // Attempts to convert config[color_name] into a color, and set var_to_set to
    // that color.  If this fails it prints an error and return false.
    bool ParseColor(map<string, string>& config,
                    const string& color_name,
                    short* var_to_set);
    bool ParseBool(map<string, string>& config,
                   const string& to_parse,
                   bool* value);

    bool ParseGeneralOptions();
    short foreground_color_;
    short background_color_;
    short header_text_color_;
    bool prompt_on_delete_task_;

    bool ParseTaskOptions();
    short unstarted_task_color_;
    short in_progress_task_color_;
    short paused_task_color_;
    short finished_task_color_;

    bool ParseMenuOptions();
    short menubar_foreground_color_;
    short menubar_background_color_;
    short unselected_menu_foreground_color_;
    short unselected_menu_background_color_;
    short selected_menu_foreground_color_;
    short selected_menu_background_color_;
};

#endif  // DONEYET_CONFIG_H_

#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

// A super simple configuration file parser.  It allows defining sections with
// [SECTION_NAME] on its own line.  Section values are of the following format:
// config_parameter=config_value
// Lines beginning with # are skipped, and padding whitespace is removed from
// the beginning and ends of config_parameters and config_values.

#include <map>
#include <string>

using std::map;
using std::string;

class ConfigParser {
  public:
    // Parse a config file and append to config.
    static bool ParseConfig(const string& config_file_path,
                            map<string, map<string, string> >* config);
};

#endif  // CONFIG_PARSER_H_

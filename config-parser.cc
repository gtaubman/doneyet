#include "config-parser.h"

#include <assert.h>

#include <fstream>
#include <iostream>

// Returns a new string that has all padding whitespace removed.
static string StripWhiteSpace(const string& s) {
  size_t start_pos = 0;
  size_t end_pos = s.length() - 1;
  while (s[start_pos] == ' ' && start_pos < s.length()) {
    ++start_pos;
  }
  while (s[end_pos] == ' ' && end_pos >= 0) {
    --end_pos;
  }
  assert(start_pos < end_pos);
  return s.substr(start_pos, end_pos - start_pos + 1);
}

bool ConfigParser::ParseConfig(const string& file_path,
                               map<string, map<string, string> >* config) {
  std::ifstream config_file(file_path.c_str());
  if (!config_file.is_open()) {
    fprintf(stderr, "Unable to open config file %s.\n", file_path.c_str());
    return false;
  }

  string line;
  map<string, string>* current_section = NULL;
  while (!config_file.eof()) {
    getline(config_file, line);

    // Skip comment lines.
    if (!line.size() || line[0] == '#') {
      continue;
    }

    // Grab this section.
    if (line[0] == '[' && line[line.length() - 1] == ']') {
      current_section = &(*config)[line.substr(1, line.length() - 2)];
      continue;
    }

    // If we get this far and we don't already have a section, bail out.  This
    // can occur when no section is provided initially.
    if (current_section == NULL) {
      fprintf(stderr, "No section at beginning of config file.\n");
      return false;
    }

    // Parse the configuration param.
    size_t equals_pos = line.find("=");
    if (equals_pos == string::npos) {
      // All configs must have an equals sign in them.
      fprintf(stderr, "Unable to parse '%s'.  Must have an = sign.\n",
              line.c_str());
      return false;
    }

    string key = StripWhiteSpace(line.substr(0, equals_pos));
    string value = StripWhiteSpace(
        line.substr(equals_pos + 1, line.size() - equals_pos - 1));
    (*current_section)[key] = value;
  }
  config_file.close();

  return true;
}

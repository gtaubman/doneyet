#include "file-manager.h"

#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>

using std::cout;
using std::endl;

FileManager::FileManager() { Initialize(); }

FileManager::~FileManager() {}

bool FileManager::Initialize() {
  // Get the user directory.  Without this we can't find where to save our data.
  char* home_dir = getenv("HOME");
  home_dir_ = string(home_dir);
  if (home_dir_.empty()) {
    cout << "Error: Unable to determine home directory." << endl;
    return false;
  }

  home_dir_ += "/";

  // Try and make the .todo directory.  This is also how we determine if it
  // already exists.
  data_dir_ = home_dir_ + ".todo";
  if (!CheckDir(data_dir_)) {
    return false;
  }
  data_dir_ += "/";

  // Make (or check for the existence of) the Projects directory.
  project_dir_ = data_dir_ + "Projects";
  if (!CheckDir(project_dir_)) {
    return false;
  }
  project_dir_ += "/";

  // See how many projects there are in the project directory
  vector<string> projects;
  if (!DirectoryContents(project_dir_, &projects)) {
    cout << "Error getting contents of project directory." << endl;
    return false;
  }

  // Check for a config file.
  config_file_path_ = data_dir_ + "/config";
  if (!FileExists(config_file_path_)) {
    config_file_path_ = "";
  }

  return true;
}

bool FileManager::DirectoryContents(const string& dir,
                                    vector<string>* contents) {
  contents->clear();
  DIR* dir_ptr = opendir(dir.c_str());
  if (dir_ptr == NULL) {
    return false;
  }

  dirent* dr;
  while ((dr = readdir(dir_ptr))) {
    if (strcmp(dr->d_name, ".") != 0 && strcmp(dr->d_name, "..") != 0 &&
        dr->d_name[0] != '.') {
      contents->push_back(dr->d_name);
    }
  }

  closedir(dir_ptr);
  return true;
}

int FileManager::NumSavedProjects() {
  vector<string> projects;
  DirectoryContents(ProjectDir(), &projects);
  return projects.size();
}

vector<string> FileManager::SavedProjectNames() {
  vector<string> projects;
  DirectoryContents(ProjectDir(), &projects);
  return projects;
}

bool FileManager::CheckDir(const string& dir) {
  int edir = mkdir(dir.c_str(), 0755);
  if (edir != 0 && errno != EEXIST) {
    cout << "Error creating or reading directory \"" << dir << "\"" << endl;
    return false;
  }
  return true;
}

bool FileManager::FileExists(const string& file_path) {
  struct stat file_stat;
  if (stat(file_path.c_str(), &file_stat) != 0) {
    return false;
  }
  return true;
}

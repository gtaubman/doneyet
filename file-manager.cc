#include "file-manager.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>

using std::cout;
using std::endl;

FileManager::FileManager() {
  Initialize();
}

FileManager::~FileManager() {

}

bool FileManager::Initialize() {
  // Get the user directory.  Without this we can't find where to save our data.
  char* home_dir = getenv("HOME");
  home_dir_ = string(home_dir);
  if (home_dir_.empty()) {
    cout << "Error: Unable to determine home directory." << endl;
    return false;
  }

  home_dir_ += "/";
  cout << "Home Directory: \"" << home_dir_ << "\"" << endl;

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
}

bool FileManager::DirectoryContents(const string& dir,
    vector<string>* contents) {
  contents->clear();
  DIR* dir_ptr = opendir(dir.c_str());
  if (dir_ptr == NULL) {
    return false;
  }

  dirent* dr;
  while (dr = readdir(dir_ptr)) {
    if (strcmp(dr->d_name, ".") != 0 &&
        strcmp(dr->d_name, "..") != 0 &&
        dr->d_name[0] != '.') {
      contents->push_back(dr->d_name);
    }
  }

  closedir(dir_ptr);
  return true;
}

bool FileManager::CheckDir(const string& dir) {
  int edir = mkdir(dir.c_str(), 0755);
  if (edir == 0) {
    cout << "Created directory \"" << dir << "\"" << endl;
  } else if (errno == EEXIST) {
    cout << "Found directory \"" << dir << "\"" << endl;
  } else {
    cout << "Error creating or reading directory \"" << dir << "\"" << endl;
    return false;
  }
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

#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class FileManager;

static FileManager* fm = NULL;

class FileManager {
 public:
  static FileManager* DefaultFileManager() {
    if (fm == NULL) {
      fm = new FileManager();
    }
    return fm;
  }

  bool Initialize();
  bool DirectoryContents(const string& dir, vector<string>* contents);

  string ProjectDir() { return project_dir_; }
  int NumSavedProjects();
  vector<string> SavedProjectNames();

  // Return the full path to the configuration file.  Will be empty if there is
  // no configuration file.
  string ConfigFilePath() { return config_file_path_; }

 private:
  FileManager();
  virtual ~FileManager();

  bool CheckDir(const string& dir);
  bool FileExists(const string& file_path);

  string home_dir_;
  string data_dir_;
  string project_dir_;
  string config_file_path_;
};

#endif  // FILE_MANAGER_H_

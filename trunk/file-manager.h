#ifndef __FILE_MANAGER__
#define __FILE_MANAGER__

#include <vector>
#include <string>

using std::vector;
using std::string;

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

 private:
  FileManager();
  virtual ~FileManager();

  bool CheckDir(const string& dir);

  string home_dir_;
  string data_dir_;
  string project_dir_;
};

#endif

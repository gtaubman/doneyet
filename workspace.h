#ifndef __WORKSPACE__
#define __WORKSPACE__

#include <assert.h>
#include <vector>
#include <string>
#include <signal.h>
#include "curses-menu.h"
#include "project.h"
#include "hierarchical-list.h"

using std::vector;
using std::string;

class StringListItem : public ListItem {
 public:
  StringListItem(string str) : ListItem() {
    str_ = str;
  }

  // Required Functions
  virtual const string TextForColumn(const string& c) { return str_; }
  virtual int NumListChildren() { return 0; }
  virtual ListItem* ListChild(int i) { assert(false); return NULL; }
  virtual ListItem* ListParent() { return NULL; }
  virtual void SetListText(string& text) { }
  
 private:
  string str_;
};

class StringVectorSource : public HierarchicalListDataSource {
 public:
  StringVectorSource() { }
  virtual ~StringVectorSource() { }

  void SetVector(const vector<string> strings) {
    for (int i = 0; i < list_items_.size(); ++i) {
      delete list_items_[i];
    }
    list_items_.clear();

    for (int i = 0; i < strings.size(); ++i) {
      list_items_.push_back(new StringListItem(strings[i]));
    }
  }
  void Clear() {
    for (int i = 0; i < list_items_.size(); ++i) {
      delete list_items_[i];
    }
    list_items_.clear();
  }

  // Required functions
  virtual int NumRoots() { return list_items_.size(); }
  virtual ListItem* Root(int i) { return list_items_[i]; }

 private:
  vector<StringListItem*> list_items_;
};

class Workspace {
 public:
  explicit Workspace();
  virtual ~Workspace();

  void Run();
  void Quit();

 private:
  void NewProject();
  void OpenProject();
  void SaveCurrentProject();

  void InitializeLists();

  // UI Actions
  void AddTask(Task* t);
  void MoveTask(Task* t);
  void ShowMenuBar(Task* t);
  void AddNote(Task* t);
  void ViewNotes(Task* t);
  void ToggleStatus(Task* t);

  // UI Helper Functions
  void HandleMenuInput(const string& input);
  void RunFind();
  void PerformFullListUpdate();
  void DisplayNotes(Task* t);

  // Initialization Helper Functions
  void InitializeMenuBar();
  Project* CreateNewProject();

  // Filter Helpers
  void ShowAllTasks();
  void ShowTasksCompletedLastWeek();
  void ShowUnfinishedTasks();

  static void SigWinchHandler(int signal,
                              siginfo_t* siginfo,
                              void* signal_ucontext);
  
  MenuBar* menubar_;
  Project* project_;
  HierarchicalList* list_;
  HierarchicalList* notes_list_;
  StringVectorSource notes_source_;
  bool done_;
};

#endif

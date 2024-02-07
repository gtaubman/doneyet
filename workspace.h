#ifndef WORKSPACE_H_
#define WORKSPACE_H_

#include <assert.h>
#include <signal.h>
#include <string>
#include <vector>

#include "curses-menu.h"
#include "hierarchical-list.h"
#include "project.h"

using std::string;
using std::vector;

#define __HELPTEXT__                                                         \
  "* A - Apply the Show All Tasks filter.\n"                                 \
  "* a - Create a new task (or a subtask if pressed while a task is "        \
  "selected).\n"                                                             \
  "* M - Show the menu bar.\n"                                               \
  "  * j and k - Change menu item.\n"                                        \
  "  * l and h - Change menu.\n"                                             \
  "  * Return - Select the selected menu item.\n"                            \
  "  * Escape - Hide the menu bar.\n"                                        \
  "* m - Move the currently selected task. Note this doesn't work for root " \
  "level tasks yet.\n"                                                       \
  "  * k/u/Up Arrow - Move selected task up.\n"                              \
  "  * j/d/Down Arrow - Move selected task down.\n"                          \
  "  * Return - Place task at current position.\n"                           \
  "  * Escape - Place task to where it was originally.\n"                    \
  "* n - Add a note to the selected task.\n"                                 \
  "* v - View the notes of the selected task.\n"                             \
  "* j - Select next task.\n"                                                \
  "* k - Select previous task.\n"                                            \
  "* Escape - Select no task.\n"                                             \
  "* e - Edit selected task.\n"                                              \
  "* d - Delete selected task.\n"                                            \
  "* c - Toggle collapsed state of selected task.\n"                         \
  "* R - Apply the Show Uncompleted Tasks filter.\n"                         \
  "* C - Apply the Show Completed Tasks filter.\n"                           \
  "* f - Apply the Find Tasks filter.\n"                                     \
  "* S - Save the project.\n"                                                \
  "* Space - Toggle the status of the selected item. White is unstarted, "   \
  "green is in progress, blue is completed and red is paused.\n"             \
  "* h - Shows and closes this help dialog.\n"                               \
  "* q - Quit.\n"

class StringListItem : public ListItem {
 public:
  explicit StringListItem(string str) : ListItem() { str_ = str; }

  // Required Functions
  virtual const string TextForColumn(const string& c) { return str_; }
  virtual int NumListChildren() { return 0; }
  virtual ListItem* ListChild(int i) {
    assert(false);
    return NULL;
  }
  virtual ListItem* ListParent() { return NULL; }
  virtual void SetListText(const string& text) {}

 private:
  string str_;
};

class StringVectorSource : public HierarchicalListDataSource {
 public:
  StringVectorSource() {}
  virtual ~StringVectorSource() {}

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
  void DisplayHelp();

  // Initialization Helper Functions
  void InitializeMenuBar();
  Project* CreateNewProject();

  // Filter Helpers
  void ShowAllTasks();
  void ShowTasksCompletedLastWeek();
  void ShowUnfinishedTasks();

  static void SigWinchHandler(int signal, siginfo_t* siginfo,
                              void* signal_ucontext);

  MenuBar* menubar_;
  Project* project_;
  HierarchicalList* list_;
  HierarchicalList* notes_list_;
  StringVectorSource notes_source_;
  bool done_;
};

#endif  // WORKSPACE_H_

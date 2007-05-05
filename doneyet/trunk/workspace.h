#ifndef __WORKSPACE__
#define __WORKSPACE__

#include "curses-menu.h"
#include "project.h"
#include "hierarchical-list.h"

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

  // Initialization Helper Functions
  void InitializeMenuBar();
  Project* CreateNewProject();
  
  MenuBar* menubar_;
  Project* project_;
  HierarchicalList* list_;
  bool done_;
};

#endif

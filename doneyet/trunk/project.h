// Copyright 2007 Google Inc. All Rights Reserved.
// Author: gtaubman@google.com (Gabriel Taubman)

#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <ncurses.h>
#include <panel.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "task.h"
#include "serializer.h"

using std::string;
using std::vector;
using std::ofstream;
using std::ifstream;

class Project : HierarchicalListDataSource {
 public:
  explicit Project(string name);
  virtual ~Project();

  static Project* NewProject();
  static Project* NewProjectFromFile(string path);

  string Name() { return name_; }
  void DrawInWindow(WINDOW* win);
  Task* AddTaskNamed(const string& name);
  void Serialize(Serializer* s);
  int NumTasks();
  void DeleteTask(Task* t);

  // Functions required by HierarchicalListDataSource:
  int NumRoots() { return tasks_.size(); }
  ListItem* Root(int i) { return static_cast<ListItem*>(tasks_[i]); }
  int NumColumns() { return 1; }

 private:
  void Rename();
  void ComputeNodeStatus();
  TaskStatus ComputeStatusForTask(Task* t);

  string name_;
  vector<Task*> tasks_;
  HierarchicalList* list_;
};

#endif

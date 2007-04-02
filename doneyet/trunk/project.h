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
#include "filter-predicate.h"
#include "curses-menu.h"

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

  void FilterTasks(FilterPredicate<Task>* filter);
  void FilterTasks() { FilterTasks(&base_filter_); }
  void RunSearchFilter(const string& find);

  string Name() { return name_; }
  void DrawInWindow(WINDOW* win);
  Task* AddTaskNamed(const string& name);
  void Serialize(Serializer* s);

  // A count of every item in the tree.
  int NumTasks();
  void DeleteTask(Task* t);
  void ShowAllTasks();
  void ShowCompletedLastWeek();
  void ArchiveCompletedTasks();
  int NumFilteredRoots();
  int NumTotalRoots() { return tasks_.size(); }
  Task* FilteredRoot(int r);

  // Functions required by HierarchicalListDataSource:
  int NumRoots() { return NumFilteredRoots(); }
  ListItem* Root(int i) { return static_cast<ListItem*>(FilteredRoot(i)); }

  void HandleMenuInput(const string& input);
 private:
  void Rename();
  void ComputeNodeStatus();
  TaskStatus ComputeStatusForTask(Task* t);

  string name_;
  vector<Task*> tasks_;
  vector<Task*> filtered_tasks_;
  HierarchicalList* list_;
  AndFilterPredicate<Task> base_filter_;
  MenuBar* menubar_;
};

#endif

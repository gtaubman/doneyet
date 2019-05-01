#ifndef PROJECT_H_
#define PROJECT_H_

#include <fstream>
#include <iostream>
#include <string>
#include "filter-predicate.h"
#include "serializer.h"
#include "task.h"

using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

class Project : public HierarchicalListDataSource {
 public:
  explicit Project(string name);
  virtual ~Project();

  static Project* NewProjectFromFile(string path);

  void FilterTasks(FilterPredicate<Task>* filter);
  void FilterTasks() { FilterTasks(&base_filter_); }
  void RunSearchFilter(const string& find);

  string Name() { return name_; }
  void Serialize(Serializer* s);
  Task* GetRootTask() { return tasks_; };

  // A count of every item in the tree.
  void DeleteTask(Task* t);

  // Various Common Filters
  void ShowAllTasks();
  void ShowCompletedLastWeek();
  void ArchiveCompletedTasks();

  // Functions required by HierarchicalListDataSource:
  int NumRoots() { return 1; }
  ListItem* Root(int i) { return static_cast<ListItem*>(filtered_tasks_); }

  void RecomputeNodeStatus();
  friend ostream& operator<<(ostream& out, Project& project);

 private:
  TaskStatus ComputeStatusForTask(Task* t);

  string name_;
  Task* tasks_;
  Task* filtered_tasks_;
  AndFilterPredicate<Task> base_filter_;
};

#endif  // PROJECT_H_

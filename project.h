#ifndef PROJECT_H_
#define PROJECT_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "filter-predicate.h"
#include "serializer.h"
#include "task.h"

using std::ifstream;
using std::ofstream;
using std::wostream;
using std::string;
using std::wstring;
using std::vector;

class Project : public HierarchicalListDataSource {
 public:
  explicit Project(wstring name);
  virtual ~Project();

  static Project* NewProjectFromFile(string path);

  void FilterTasks(FilterPredicate<Task>* filter);
  void FilterTasks() { FilterTasks(&base_filter_); }
  void RunSearchFilter(const wstring& find);

  wstring Name() const { return name_; }
  Task* AddTaskNamed(const wstring& name);
  void Serialize(Serializer* s);

  // A count of every item in the tree.
  int NumTasks();
  void DeleteTask(Task* t);

  // Various Common Filters
  void ShowAllTasks();
  void ShowCompletedLastWeek();
  void ArchiveCompletedTasks();

  int NumFilteredRoots() const;
  Task* FilteredRoot(int r);

  // Functions required by HierarchicalListDataSource:
  int NumRoots() { return NumFilteredRoots(); }
  ListItem* Root(int i) { return static_cast<ListItem*>(FilteredRoot(i)); }

  void RecomputeNodeStatus();
  friend ostream& operator<<(ostream& out, Project& project);

 private:
  TaskStatus ComputeStatusForTask(Task* t);

  wstring name_;
  vector<Task*> tasks_;
  vector<Task*> filtered_tasks_;
  AndFilterPredicate<Task> base_filter_;
};

#endif  // PROJECT_H_

#ifndef PROJECT_H_
#define PROJECT_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "task.h"
#include "serializer.h"
#include "filter-predicate.h"

using std::string;
using std::vector;
using std::ofstream;
using std::ifstream;

class Project : public HierarchicalListDataSource {
  public:
    explicit Project(string name);
    virtual ~Project();

    static Project* NewProjectFromFile(string path);

    void FilterTasks(FilterPredicate<Task>* filter);
    void FilterTasks() {
        FilterTasks(&base_filter_);
    }
    void RunSearchFilter(const string& find);

    string Name() {
        return name_;
    }
    Task* AddTaskNamed(const string& name);
    void Serialize(Serializer* s);

    // A count of every item in the tree.
    int NumTasks();
    void DeleteTask(Task* t);

    // Various Common Filters
    void ShowAllTasks();
    void ShowCompletedLastWeek();
    void ArchiveCompletedTasks();

    int NumFilteredRoots();
    Task* FilteredRoot(int r);

    // Functions required by HierarchicalListDataSource:
    int NumRoots() {
        return NumFilteredRoots();
    }
    ListItem* Root(int i) {
        return static_cast<ListItem*>(FilteredRoot(i));
    }

    void RecomputeNodeStatus();
    friend ostream& operator <<(ostream& out, Project& project);

  private:
    TaskStatus ComputeStatusForTask(Task* t);

    string name_;
    vector<Task*> tasks_;
    vector<Task*> filtered_tasks_;
    AndFilterPredicate<Task> base_filter_;
};

#endif  // PROJECT_H_

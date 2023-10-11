#ifndef TASK_H_
#define TASK_H_

// TODO: Swap(Task*, Task*) is still a bit flakey if the tasks are in the
// "wrong" order.  Fix that.

#include <ncursesw/ncurses.h>

#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "date.h"
#include "filter-predicate.h"
#include "hierarchical-list.h"
#include "note.h"
#include "serializer.h"

using std::map;
using std::ofstream;
using std::wostream;
using std::wstring;
using std::vector;

typedef enum TaskStatus_ {
  CREATED,
  PAUSED,
  IN_PROGRESS,
  COMPLETED,
  NUM_STATUSES,
} TaskStatus;

class Task : public ListItem {
 public:
  Task(const wstring &title, const wstring &description);
  virtual ~Task();
  static Task* NewTaskFromSerializer(Serializer* s);

  bool HasNotes();
  void AddNote(const wstring& note);
  void DeleteNote(const wstring& note);
  vector<wstring> Notes();
  map<wstring, wstring> MappedNotes();

  wstring Title() { return title_; }
  static wstring TitleWrapper(Task* t) { return t->Title(); }
  wstring Description() { return description_; }
  static wstring DescriptionWrapper(Task* t) { return t->Description(); }
  Date CompletionDate() { return completion_date_; }
  static time_t CompletionDateWrapper(Task* t) {
    return t->CompletionDate().Time();
  }

  void ApplyFilter(FilterPredicate<Task>* filter);
  void AddSubTask(Task* subtask);
  void SetParent(Task* p) { parent_ = p; }
  void RemoveSubtaskFromList(Task* t);
  void Delete();
  void DeleteTask(Task* t);
  void SwapTasks(Task* a, Task* b);
  void MoveTaskUp(Task* t);
  void MoveTaskDown(Task* t);
  void MoveUp();
  void MoveDown();

  void SetStatus(TaskStatus t);
  TaskStatus Status() { return status_; }
  static TaskStatus StatusWrapper(Task* t) { return t->Status(); }

  // Serializes this task and all of its children.
  void Serialize(Serializer* s);

  // Returns the number of tasks below this task.
  int NumOffspring();
  int NumFilteredOffspring();
  static int NumFilteredOffspringWrapper(Task* t) {
    return t->NumFilteredOffspring();
  }

  int NumChildren() { return subtasks_.size(); }
  Task* Child(int i) { return subtasks_[i]; }
  int NumFilteredChildren();
  Task* FilteredChild(int c);
  Task* Parent() { return parent_; }

  // Functions required by list item
  const wstring TextForColumn(const wstring& c) {
    if (c == L"Task") return title_;
    if (c == L"Created") return convToWstring(creation_date_.ToString());
    if (c == L"Completed") {
      if (completion_date_.Time() == 0) return L"";
      return convToWstring(completion_date_.ToString());
    }
    if (c == L"N") return (notes_.size() ? L"X" : L"");
    return L"UNKNOWN";
  }
  int ListColor();
  int NumListChildren() { return NumFilteredChildren(); }
  Task* ListChild(int c) { return FilteredChild(c); }
  Task* ListParent() { return Parent(); }
  void SetListText(const wstring& text) { title_ = text; }

  void ToStream(wostream& out, int depth);

 private:
  friend class Project;
  void UnSerializeFromSerializer(Serializer* s);

  Task* parent_;
  TaskStatus status_;
  vector<Task*> subtasks_;
  vector<Task*> filtered_tasks_;
  wstring title_;
  wstring description_;
  Date creation_date_;
  Date start_date_;
  Date completion_date_;
  vector<Note*> notes_;

  // Keep track of any changes to the status of a task.
  struct StatusChange {
    StatusChange(const Date& d, int s)
        : date(d), status(static_cast<TaskStatus>(s)) {}
    Date date;
    TaskStatus status;
  };
  vector<StatusChange> status_changes_;
};

#endif  // TASK_H_

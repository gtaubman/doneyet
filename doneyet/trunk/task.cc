#include <string>
#include "task.h"
#include "utils.h"
#include "file-versions.h"

using std::string;

Task::Task(const string& title,
           const string& description) 
    : parent_(NULL),
      status_(CREATED),
      title_(title),
      description_(description) {
  creation_date_.SetToNow();
  start_date_.SetToEmptyTime();
  completion_date_.SetToEmptyTime();
}

Task::~Task() {
  for (int i = 0; i < subtasks_.size(); ++i) {
    delete subtasks_[i];
  }
}

Task* Task::NewTaskFromSerializer(Serializer* s) {
  string title = s->ReadString();
  string description = s->ReadString();
  Task* t = new Task(title, description);
  t->UnSerializeFromSerializer(s);
  return t;
}

void Task::AddNote(const string& note) {
  notes_.push_back(new Note(note));
}

vector<string> Task::Notes() {
  vector<string> notes;
  for (int i = 0; i < notes_.size(); ++i) {
    beep();
    notes.push_back(notes_[i]->Text());
  }
  return notes;
}

void Task::ApplyFilter(FilterPredicate<Task>* filter) {
  // It's important that we filter ourselves after our children because often
  // filters have an OrPredicate of "Has any filtered children" which wouldn't
  // if we filtered ourselves before our children.
  for (int i = 0; i < NumChildren(); ++i) {
    Child(i)->ApplyFilter(filter);
  }
  filtered_tasks_ = filter->FilterVector(subtasks_);
}

void Task::AddSubTask(Task* subtask) {
  subtask->SetParent(this);
  subtasks_.push_back(subtask);
}

void Task::RemoveSubtaskFromList(Task* t) {
  for (int i = 0; i < subtasks_.size(); ++i) {
    if (subtasks_[i] == t) {
      subtasks_.erase(subtasks_.begin() + i);
      return;
    }
  }
}

void Task::DeleteTask(Task* t) {
  // Check if we're supposed to delete ourselves:
  if (this == t) {
    Delete();
  } else {
    for (int i = 0; i < NumChildren(); ++i) {
      if (Child(i) == t) {
        Child(i)->Delete();
        return;
      } else {
        Child(i)->DeleteTask(t);
      }
    }
  }
}

void Task::Delete() {
  for (int i = 0; i < NumChildren(); ++i) {
    Child(i)->Delete();
  }
  if (Parent() != NULL) {
    Parent()->RemoveSubtaskFromList(this);
  }
  delete this;
}

void Task::SwapTasks(Task* a, Task* b) {
  if (a == b) {
    return;
  }
  
  // Find the indices of a and b.
  vector<Task*>::iterator ait = find(subtasks_.begin(), subtasks_.end(), a);
  vector<Task*>::iterator bit = find(subtasks_.begin(), subtasks_.end(), b);

  if (ait == subtasks_.end() ||
      bit == subtasks_.end()) {
    return;
  }

  int ai = ait - subtasks_.begin();
  int bi = bit - subtasks_.begin();

  // Remove B from the vector.
  subtasks_.erase(bit);

  // Place B before A.
  subtasks_.insert(subtasks_.begin() + ai, b);

  // Remove A from the vector (A is now one further forward because B was added)
  subtasks_.erase(subtasks_.begin() + ai + 1);

  // Place A where B used to be
  subtasks_.insert(subtasks_.begin() + bi, a);
}

void Task::MoveTaskUp(Task* t) {
  // We can't move up the first task.
  if (t == filtered_tasks_[0]) {
    return;
  }

  // Find the task in our filtered list:
  vector<Task*>::iterator it = find(filtered_tasks_.begin(),
                                    filtered_tasks_.end(),
                                    t);
  SwapTasks(*(--it), t);
}

void Task::MoveTaskDown(Task* t) {
  // We can't move down the last task.
  if (t == filtered_tasks_[filtered_tasks_.size() - 1]) {
    return;
  }

  // Find the task in our filtered list:
  vector<Task*>::iterator it = find(filtered_tasks_.begin(),
                                    filtered_tasks_.end(),
                                    t);
  SwapTasks(t, *(++it));
}

void Task::MoveUp() {
  if (Parent()) {
    Parent()->MoveTaskUp(this);
  }
}

void Task::MoveDown() {
  if (Parent()) {
    Parent()->MoveTaskDown(this);
  }
}

void Task::SetTitle(const string& title) {
  title_ = title;
}

void Task::SetDescription(const string& description) {
  description_ = description;
}

void Task::Serialize(Serializer* s) {
  // Initially we store a unique identifier to ourselves that will help with
  // reading in the tasks and assembling the tree.
  s->WriteUint64((uint64)this);

  // Data about this task.
  s->WriteString(title_);
  s->WriteString(description_);
  s->WriteInt32(static_cast<int32>(status_));

  // Various dates.
  creation_date_.Serialize(s);
  start_date_.Serialize(s);
  completion_date_.Serialize(s);

  // The notes associated with this task.
  s->WriteInt32(notes_.size());
  for (int i = 0; i < notes_.size(); ++i) {
    notes_[i]->Serialize(s);
  }

  // Finally our parent pointer and then we move onto the children.
  s->WriteUint64((uint64)parent_);
  for (int i = 0; i < subtasks_.size(); ++i) {
    subtasks_[i]->Serialize(s);
  }
}

void Task::UnSerializeFromSerializer(Serializer* s) {
  status_ = static_cast<TaskStatus>(s->ReadInt32());
  creation_date_.ReadFromSerializer(s);
  start_date_.ReadFromSerializer(s);
  completion_date_.ReadFromSerializer(s);

  if (s->Version() >= NOTES_VERSION) {
    int num_notes = s->ReadInt32();
    for (int i = 0; i < num_notes; ++i) {
      Note* n = new Note("");
      n->ReadFromSerializer(s);
      notes_.push_back(n);
    }
  }
}

void Task::SetStatus(TaskStatus t) {
  if (status_ == CREATED && t == IN_PROGRESS) {
    // We were set to in progress for the first time.
    start_date_.SetToNow();
  } else if (t == COMPLETED && status_ != COMPLETED) {
    completion_date_.SetToNow();
  } else if (t == PAUSED && status_ != PAUSED) {
    completion_date_.SetToEmptyTime();
  }
  if (t == IN_PROGRESS)
    completion_date_.SetToEmptyTime();

  status_ = t;
}

int Task::NumOffspring() {
  int sum_from_children = 0;
  for (int i = 0; i < subtasks_.size(); ++i) {
    sum_from_children += 1 + subtasks_[i]->NumOffspring();
  }

  return sum_from_children;
}

int Task::NumFilteredOffspring() {
  int sum_from_children = 0;
  for (int i = 0; i < filtered_tasks_.size(); ++i) {
    sum_from_children += 1 + filtered_tasks_[i]->NumFilteredOffspring();
  }
  return sum_from_children;
}

int Task::ListColor() {
  int c = 0;
  if (status_ == CREATED) {
    c |= COLOR_PAIR(0);
  } else if (status_ == IN_PROGRESS) {
    c |= COLOR_PAIR(2);  // green
  } else if (status_ == COMPLETED) {
    c |= COLOR_PAIR(4);  // blue
  } else if (status_ == PAUSED) {
    c |= COLOR_PAIR(1);  // red
  }

  return c;
}

int Task::NumFilteredChildren() {
  return filtered_tasks_.size();
}

Task* Task::FilteredChild(int c) {
  return filtered_tasks_[c];
}

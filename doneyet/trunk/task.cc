#include <string>
#include "task.h"
#include "utils.h"

using std::string;

Task::Task(const string& title,
           const string& description) 
    : parent_(NULL),
      status_(CREATED),
      title_(title),
      description_(description),
      archived_(false) {
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

void Task::SetTitle(const string& title) {
  title_ = title;
}

void Task::SetDescription(const string& description) {
  description_ = description;
}

void Task::Serialize(Serializer* s) {
  s->WriteUint64((uint64)this);
  s->WriteString(title_);
  s->WriteString(description_);
  s->WriteInt32(static_cast<int32>(status_));
  creation_date_.Serialize(s);
  start_date_.Serialize(s);
  completion_date_.Serialize(s);
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
}

void Task::SetStatus(TaskStatus t) {
  if (status_ == CREATED && t == IN_PROGRESS) {
    // We were set to in progress for the first time.
    start_date_.SetToNow();
  } else if (t == COMPLETED) {
    completion_date_.SetToNow();
  } else if (t == PAUSED) {
    completion_date_.SetToEmptyTime();
  }

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

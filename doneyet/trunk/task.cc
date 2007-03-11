#include <string>
#include "task.h"
#include "utils.h"

using std::string;

Task::Task(const string& title,
           const string& description) 
    : parent_(NULL),
      status_(CREATED),
      title_(title),
      description_(description) {
  creation_date_.SetToNow();
  completion_date_.SetToEmptyTime();
}

Task::~Task() {
  for (int i = 0; i < subtasks_.size(); ++i) {
    delete subtasks_[i];
  }
}

void Task::AddSubTask(Task* subtask) {
  subtask->SetParent(this);
  subtasks_.push_back(subtask);
}

void Task::SetTitle(const string& title) {
  title_ = title;
}

void Task::SetDescription(const string& description) {
  description_ = description;
}

void Task::Serialize(Serializer* s) {
  std::cout << "Serializing \"" << title_ << "\"" << std::endl;
  s->Write((int)this);
  s->Write(title_);
  s->Write(description_);
  s->Write((int) status_);
  s->Write((int) parent_);
  for (int i = 0; i < subtasks_.size(); ++i) {
    subtasks_[i]->Serialize(s);
  }
}

int Task::NumOffspring() {
  int sum_from_children = 0;
  for (int i = 0; i < subtasks_.size(); ++i) {
    sum_from_children += subtasks_[i]->NumOffspring();
  }

  return 1 + sum_from_children;
}

int Task::Color() {
  int c = 0;
  if (status_ == CREATED) {
    c |= COLOR_PAIR(0) | A_DIM;
  } else {
    c |= COLOR_PAIR(status_);
  }

  return c;
}

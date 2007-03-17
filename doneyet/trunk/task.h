#ifndef __TASK_H__
#define __TASK_H__

#include <ncurses.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include "hierarchical-list.h"
#include "serializer.h"
#include "date.h"

using std::string;
using std::vector;
using std::ofstream;

typedef enum TaskStatus_ {
  CREATED,
  PAUSED,
  IN_PROGRESS,
  COMPLETED,
  NUM_STATUSES,
} TaskStatus;

class Task : public ListItem {
 public:
   Task(const string& title,
        const string& description);
   virtual ~Task();
   static Task* NewTaskFromSerializer(Serializer* s);

   void SetTitle(const string& title);
   void SetDescription(const string& description);
   
   void AddSubTask(Task* subtask);
   void SetParent(Task* p) { parent_ = p; }
   void RemoveSubtaskFromList(Task* t);
   void Delete();
   void DeleteTask(Task* t);
   void SetStatus(TaskStatus t);
   TaskStatus Status() { return status_; }

   // Serializes this task and all of its children.
   void Serialize(Serializer* s);
  
   // Returns the number of tasks below this task.
   int NumOffspring();

   // Functions required by list item
   const string Text() { return title_; }
   int Color();
   int NumChildren() { return subtasks_.size(); }
   Task* Child(int i) { return subtasks_[i]; }
   Task* Parent() { return parent_; }
   void SetText(string& text) { title_ = text; }


 private:
   friend class Project;
   void UnSerializeFromSerializer(Serializer* s);
   
   Task* parent_;
   TaskStatus status_;
   vector<Task*> subtasks_;
   string title_;
   string description_;
   Date creation_date_;
   Date start_date_;
   Date completion_date_;
};

#endif

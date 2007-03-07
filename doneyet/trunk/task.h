#ifndef __TASK_H__
#define __TASK_H__

#include <ncurses.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "hierarchical-list.h"
#include "serializer.h"

using std::string;
using std::vector;
using std::ofstream;

class Task : public ListItem{
 public:
   Task(const string& title,
        const string& description);
   virtual ~Task();

   void SetTitle(const string& title);
   void SetDescription(const string& description);
   
   void AddSubTask(Task* subtask);
   void SetParent(Task* p) { parent_ = p; }

   // Serializes this task and all of its children.
   void Serialize(Serializer* s);
  
   // Returns the number of tasks below this task.
   int NumOffspring();

   // Functions required by list item
   const string Text() { return title_; }
   bool ShouldExpand() { return true; }
   int NumChildren() { return subtasks_.size(); }
   ListItem* Child(int i) { return subtasks_[i]; }
   ListItem* Parent() { return parent_; }
   void SetText(string& text) { title_ = text; }


 private:
   Task* parent_;
   bool completed_;
   vector<Task*> subtasks_;
   string title_;
   string description_;
};

#endif

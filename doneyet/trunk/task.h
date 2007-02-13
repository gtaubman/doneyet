#ifndef __TASK_H__
#define __TASK_H__

#include <ncurses.h>
#include <string>
#include <vector>
#include "hierarchical-list.h"

using std::string;
using std::vector;

class Task : public ListItem{
 public:
   Task(const string& title,
        const string& description);
   virtual ~Task();

   void SetTitle(const string& title);
   void SetDescription(const string& description);
   
   void AddSubTask(Task* subtask);
   int DrawInWindow(WINDOW* win, int line_num, int indent);

   void SetParent(Task* p) { parent_ = p; }
  
   // Functions required by list item
   const string Text() { return title_; }
   bool ShouldExpand() { return true; }
   int NumChildren() { return subtasks_.size(); }
   ListItem* Child(int i) { return subtasks_[i]; }
   ListItem* Parent() { return parent_; }
   void AppendCharacter(char* c) { title_.append(c); }

 //private:
   Task* parent_;
   vector<Task*> subtasks_;
   string title_;
   string description_;
};

#endif

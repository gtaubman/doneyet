// Copyright 2007 Google Inc. All Rights Reserved.
// Author: gtaubman@google.com (Gabriel Taubman)

#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <ncurses.h>
#include <panel.h>
#include <string>
#include <vector>
#include "task.h"

using std::string;
using std::vector;

class Project {
 public:
  explicit Project(string name);
  virtual ~Project();

  static Project* NewProject();

  void DrawInWindow(WINDOW* win);
  Task* AddTaskNamed(const string& name);

  string name_;
  vector<Task*> tasks_;
};

#endif

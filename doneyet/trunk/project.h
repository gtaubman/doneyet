// Copyright 2007 Google Inc. All Rights Reserved.
// Author: gtaubman@google.com (Gabriel Taubman)

#ifndef __PROJECT_H__
#define __PROJECT_H__

#include "cdk/cdk.h"
#include <string>

using std::string;

class Project {
 public:
  explicit Project(string name);
  virtual ~Project();

  static Project* NewProject(CDKSCREEN* screen);

  string name_;
};

#endif

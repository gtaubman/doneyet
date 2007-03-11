#ifndef __TASK__DATE__
#define __TASK__DATE__

#include <ctime>
#include <string>

using std::time_t;
using std::string;

class Date {
 public:
  explicit Date();
  virtual ~Date();

  void SetToNow();
  void SetToEmptyTime();

  string ToString();

 private:
  time_t time_;
};

#endif

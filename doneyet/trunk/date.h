#ifndef __TASK__DATE__
#define __TASK__DATE__

#include <ctime>
#include <string>
#include "serializer.h"

using std::time_t;
using std::string;

class Date {
 public:
  explicit Date();
  virtual ~Date();

  void SetToNow();
  void SetToEmptyTime();

  void Serialize(Serializer* s);
  void ReadFromSerializer(Serializer* s);
  
  string ToString();

 private:
  time_t time_;
};

#endif

#ifndef DATE_H_
#define DATE_H_

#include <ctime>
#include <string>

#include "serializer.h"

using std::string;
using std::time_t;

class Date {
 public:
  explicit Date();
  virtual ~Date();

  void SetToNow();
  void SetToEmptyTime();
  time_t Time() const { return time_; }

  void Serialize(Serializer* s) const;
  void ReadFromSerializer(Serializer* s);

  string ToString() const;

 private:
  time_t time_;
};

std::wstring convToWstring(const std::string& src);

#endif  // DATE_H_

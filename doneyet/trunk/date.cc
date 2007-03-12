#include "date.h"

Date::Date() {
  std::time(&time_);
}

Date::~Date() {
  // Nothing to delete for now.
}

void Date::SetToNow() {
  std::time(&time_);
}

void Date::SetToEmptyTime() {
  time_ = 0;
}

string Date::ToString() {
  return string(ctime(&time_));
}

void Date::Serialize(Serializer* s) {
  s->Write(static_cast<int>(time_));
}

void Date::ReadFromSerializer(Serializer* s) {
  time_ = s->ReadInt();
}

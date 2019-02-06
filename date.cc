#include "date.h"
#include "utils.h"

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

bool Date::IsCloserToNowThan(Date* d) {
    return time_ > d->Time();
}

string Date::ToString() {
    // For some reason, ctime and its friends output the strings with a newline in
    // them:
    // Thu Nov 24 18:22:48 1986\n\0
    // So, we turn the newline into a NULL
    char buf[26];
    ctime_r(&time_, buf);
    buf[24] = '\0';
    return string(buf);
}

void Date::Serialize(Serializer* s) {
    s->WriteInt32(static_cast<int32>(time_));
}

void Date::ReadFromSerializer(Serializer* s) {
    time_ = s->ReadInt32();
}

#ifndef __TASK_DATE__
#define __TASK_DATE__

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
    bool IsCloserToNowThan(Date* d);
    time_t Time() {
        return time_;
    }

    void Serialize(Serializer* s);
    void ReadFromSerializer(Serializer* s);

    string ToString();

  private:
    time_t time_;
};

#endif

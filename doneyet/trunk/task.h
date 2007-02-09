#ifndef __TASK_H__
#define __TASK_H__

#include <vector>

class Task {
 public:
   Task();
   virtual ~Task();

   void AddSubTask(Task* subtask);
};

#endif

#include "task.h"
#include <iostream>

Task::Task() {
  std::cout << "Made a new task." << std::endl;
}

Task::~Task() {

}

void Task::AddSubTask(Task* subtask) {

}

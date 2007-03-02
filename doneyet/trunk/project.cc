#include "project.h"
#include "dialog-box.h"
#include "hierarchical-list.h"
#include "utils.h"
#include <map>

using std::map;
using std::cout;
using std::endl;

Project::Project(string name)
  : name_(name) {
}

Project::~Project() {
  for (int i = 0; i < tasks_.size(); ++i) {
    delete tasks_[i];
  }
}

Project* Project::NewProject() {
  // Ask for a project name
  string answer = DialogBox::RunCentered(
      "Please Enter a Project Name",
      "Default Project");
  return new Project(answer);
}

void Project::DrawInWindow(WINDOW* win) {
  window_info info = get_window_info(win);
  HierarchicalList* list = new HierarchicalList(name_,
      info.height,
      info.width,
      0,
      0);
  vector<ListItem*> roots;
  for (int i = 0; i < tasks_.size(); ++i) {
    roots.push_back((ListItem*) tasks_[i]);
  }
  list->SetDatasource(&roots);
  list->Draw();

  int ch;
  bool done = false;
  //while (!done && (ch = list->GetInput())) {
  string tmp_str;
  while (!done && (ch = getch())) {
    switch (ch) {
      case 'a':
      case 'A':
        tmp_str = DialogBox::RunMultiLine("Enter New Task",
            "",
            winwidth(win) / 3,
            winheight(win) / 3);
        if (!tmp_str.empty()) {
          Task* t = new Task(tmp_str, "");
          if (list->SelectedItem() == NULL) {
            tasks_.push_back(t);
            roots.push_back(static_cast<ListItem*>(t));
          } else {
            static_cast<Task*>(list->SelectedItem())->AddSubTask(t);
          }
        }
        break;
      case 'j':
        list->SelectNextItem();
        break;
      case 'k':
        list->SelectPrevItem();
        break;
      case 'e':
        list->EditSelectedItem();
        break;
      case 27: // escape
        list->SelectNoItem();
        break;
      case '\r':
        done = true;
        return;
    }
    list->Draw();
  }
}

Task* Project::AddTaskNamed(const string& name) {
  Task* nt = new Task(name, "");
  tasks_.push_back(nt);  
  return nt;
}

void Project::Serialize(Serializer* s) {
  // Write our project name to the file.
  s->Write(name_);

  // Write how many tasks there are.
  s->Write(NumTasks());

  // Serialize the tree.
  for (int i = 0; i < tasks_.size(); ++i) {
    tasks_[i]->Serialize(s);
  }
}

Project* Project::NewProjectFromFile(string path) {
  // Create the serializer
  printf("Going to open \"%s\"\n", path.c_str());
  Serializer s(path, "");
  if (!s.Okay()) {
    cout << "Error opening saved project: \"" << path << "\"" << endl;
    return NULL;
  }

  // Find the project's name
  string project_name = s.ReadString();
  printf("PROJECT NAME: %s\n", project_name.c_str());
  Project* p = new Project(project_name);

  // Find how many tasks there are.
  int num_tasks = s.ReadInt();
  
  // First read in every task in the file.
  int pointer_val;
  string title;
  string description;
  bool completed;
  int parent_pointer;
  map<int, Task*> task_map;
  map<Task*, int> tasks_parents;
  vector<Task*> tasks;
  for (int i = 0; i < num_tasks; ++i) {
    pointer_val = s.ReadInt();
    title = s.ReadString();
    description = s.ReadString();
    completed = (bool) s.ReadInt();
    parent_pointer = s.ReadInt();
    cout << "Read task " << title << ", " << description << ". Parent: " <<
      parent_pointer << ", Self: " << pointer_val << endl;
    Task* t = new Task(title, description);
    tasks_parents[t] = parent_pointer;
    task_map[pointer_val] = t;
    tasks.push_back(t);
  }
  
  // Then re-assemble the tree structure.
  for (int i = 0; i < tasks.size(); ++i) {
    Task* t = tasks[i];
    if (tasks_parents[t] == 0) {
      // We have a root task.  Add it to the root list.
      printf("Found a root task\n");
      p->tasks_.push_back(tasks[i]);
    } else {
      // We have a child task.  Add it to its parent's list.
      printf("Found a subtask.\n");
      task_map[tasks_parents[t]]->AddSubTask(tasks[i]);
    }
  }

  return p;
}

int Project::NumTasks() {
  int total = 0;
  for (int i = 0; i < tasks_.size(); ++i) {
    total += tasks_[i]->NumOffspring();
  }
  return total;
}

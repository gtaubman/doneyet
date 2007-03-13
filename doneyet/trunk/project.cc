#include "project.h"
#include <map>
#include "dialog-box.h"
#include "hierarchical-list.h"
#include "utils.h"

using std::map;
using std::cout;
using std::endl;

Project::Project(string name)
  : name_(name),
    list_(NULL) {
}

Project::~Project() {
  for (int i = 0; i < tasks_.size(); ++i) {
    tasks_[i]->Delete();
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
  list_ = new HierarchicalList(name_,
      info.height,
      info.width,
      0,
      0);
  list_->SetDatasource(this);
  list_->Draw();

  int ch;
  bool done = false;
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
          if (list_->SelectedItem() == NULL) {
            tasks_.push_back(t);
          } else {
            static_cast<Task*>(list_->SelectedItem())->AddSubTask(t);
          }
        }
        ComputeNodeStatus();
        break;
      case 'j':
      case KEY_DOWN:
        list_->SelectNextItem();
        break;
      case KEY_UP:
      case 'k':
        list_->SelectPrevItem();
        break;
      case 'r':
        Rename();
        break;
      case 'e':
        list_->EditSelectedItem();
        break;
      case 'd':
        Task* si = static_cast<Task*>(list_->SelectedItem());
        list_->SelectPrevItem();
        DeleteTask(si);
        ComputeNodeStatus();
        break;
      case 'c':
        list_->ToggleExpansionOfSelectedItem();
        break;
      case 27: // escape
        list_->SelectNoItem();
        break;
      case 32: // space
        if (!static_cast<Task*>(list_->SelectedItem())->NumChildren()) {
          switch (static_cast<Task*>(list_->SelectedItem())->Status()) {
            case CREATED:
            case PAUSED:
              static_cast<Task*>(list_->SelectedItem())->SetStatus(IN_PROGRESS);
              break;
            case IN_PROGRESS:
              static_cast<Task*>(list_->SelectedItem())->SetStatus(COMPLETED);
              break;
            case COMPLETED:
              static_cast<Task*>(list_->SelectedItem())->SetStatus(PAUSED);
              break;
          }
          ComputeNodeStatus();
        }
        break;
      case '\r':
        done = true;
        return;
    }
    list_->Draw();
  }
}

Task* Project::AddTaskNamed(const string& name) {
  Task* nt = new Task(name, "");
  tasks_.push_back(nt);  
  return nt;
}

void Project::Serialize(Serializer* s) {
  // Write our project name to the file.
  s->WriteString(name_);

  // Write how many tasks there are.
  s->WriteInt32(NumTasks());

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
  int num_tasks = s.ReadInt32();
  cout << "There are " << num_tasks << " tasks." << endl;
  
  // First read in every task in the file.
  int pointer_val;
  string title;
  string description;
  TaskStatus status;
  int parent_pointer;
  map<int, Task*> task_map;
  map<Task*, int> tasks_parents;
  vector<Task*> tasks;
  for (int i = 0; i < num_tasks; ++i) {
    // Read in the values.
    pointer_val = s.ReadUint64();
    title = s.ReadString();
    description = s.ReadString();
    Task* t = new Task(title, description);
    status = static_cast<TaskStatus>(s.ReadInt32());
    t->UnserializeDates(&s);

    parent_pointer = s.ReadUint64();

    // Set values in the new task.
    t->SetStatus(status);
    tasks_parents[t] = parent_pointer;
    task_map[pointer_val] = t;
    tasks.push_back(t);
  }
  
  // Then re-assemble the tree structure.
  for (int i = 0; i < tasks.size(); ++i) {
    Task* t = tasks[i];
    if (tasks_parents[t] == 0) {
      // We have a root task.  Add it to the root list_.
      p->tasks_.push_back(tasks[i]);
    } else {
      // We have a child task.  Add it to its parent's list_.
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

void Project::DeleteTask(Task* t) {
  if (t->Parent() == NULL) {
    // It's a top level task.  Remove it from our list of roots.
    for (int i = 0; i < tasks_.size(); ++i) {
      if (tasks_[i] == t) {
        tasks_[i]->Delete();
        tasks_.erase(tasks_.begin() + i);
        break;
      }
    }
  } else {
    for (int i = 0; i < tasks_.size(); ++i) {
      tasks_[i]->DeleteTask(t);
    }
  }
}

void Project::Rename() {
  string answer = DialogBox::RunCenteredWithWidth(
      "Rename Project",
      name_,
      20);
  if (!answer.empty())
    name_ = answer;
}

// Compute the status of all nodes.  Nodes which have children have their status
// for them (hence the need for this function).  A node with any IN_PROGRESS
// child is itself IN_PROGRESS.  If all of a node's children are PAUSED, the
// node is PAUSED.
void Project::ComputeNodeStatus() {
  for (int i = 0; i < tasks_.size(); ++i) {
    ComputeStatusForTask(tasks_[i]);
  }
}

TaskStatus Project::ComputeStatusForTask(Task* t) {
  if (!t->NumChildren()) {
    return t->Status();
  }

  int status_counts[NUM_STATUSES];
  for (int i = 0; i < NUM_STATUSES; ++i) {
    status_counts[i] = 0;
  }

  for (int i = 0; i < t->NumChildren(); ++i) {
    TaskStatus s = ComputeStatusForTask(t->Child(i));
    ++status_counts[s];
  }

  // If any children are in progress, so is this one.
  if (status_counts[IN_PROGRESS]) {
    t->SetStatus(IN_PROGRESS);
    return IN_PROGRESS;
  }

  // If all children are the same, this node gets that status.
  for (int i = 0; i < NUM_STATUSES; ++i) {
    if (status_counts[i] == t->NumChildren()) {
      t->SetStatus((TaskStatus) i);
      return (TaskStatus) i;
    }
  }

  // Under any other circumstance we're in progress.
  t->SetStatus(IN_PROGRESS);
  return IN_PROGRESS;
}

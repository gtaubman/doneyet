#include "project.h"
#include "dialog-box.h"
#include "utils.h"

Project::Project(string name)
  : name_(name) {
  // Add some bogus tasks.
    for (int i = 0; i < 5; ++i) {
  Task* t = AddTaskNamed("Get GWS Working so that it can host crowd using much better rules.  Unfortunately this is going to involve running the gws unittests over and over and over to see if I've messed up.");
  t->AddSubTask(new Task("Write the code.", ""));
  t->AddSubTask(new Task("Write unittests and run them.", ""));
  t->AddSubTask(new Task("Code review.", ""));
  t->AddSubTask(new Task("Submit.", ""));
  Task* t2 = AddTaskNamed("Get GWS Working so that it can host crowd using much better rules.  Unfortunately this is going to involve running the gws unittests over and over and over to see if I've messed up.");
  t2->AddSubTask(new Task("Write the code.", ""));
  t2->AddSubTask(new Task("Write unittests and run them.", ""));
  t2->AddSubTask(new Task("Code review.", ""));
  t2->AddSubTask(new Task("Submit.", ""));
    }
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
  Project* p = NULL;
  if (!answer.empty()) {
    p = new Project(answer);
  }
  return p;
}

void Project::DrawInWindow(WINDOW* win) {
  window_info info = get_window_info(win);
  HierarchicalList* list = new HierarchicalList(name_, info.height, info.width, 0, 0);
  vector<ListItem*> roots;
  for (int i = 0; i < tasks_.size(); ++i) {
    roots.push_back((ListItem*) tasks_[i]);
  }
  list->SetDatasource(roots);
  list->Run();
  return;
  

  // First draw the name of the project centered at the top of the window
  print_in_middle(win, 1, 0, info.width, name_.c_str(), COLOR_PAIR(2));

  // Scoot us over one and down one so we don't draw over the box lines
  ++info.origin_x;
  ++info.origin_y;
  info.width -= 1;

  int lines_to_draw_on = 3;
  for (int i = 0; i < tasks_.size(); ++i) {
    lines_to_draw_on += tasks_[i]->DrawInWindow(win, lines_to_draw_on, 5);
  }
}

Task* Project::AddTaskNamed(const string& name) {
  Task* nt = new Task(name, "");
  tasks_.push_back(nt);  
  return nt;
}

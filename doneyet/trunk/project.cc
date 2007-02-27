#include "project.h"
#include "dialog-box.h"
#include "utils.h"

Project::Project(string name)
  : name_(name) {
  // Add some bogus tasks.
    /*
    Task* l = AddTaskNamed("Temp Task");
    for (int i = 0; i < 20; ++i) {
      Task* s = new Task("Temp Task", "");
      l->AddSubTask(s);
      l = s;
    }
     
  for (int i = 0; i < 200; ++i) {
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
  }*/
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
  HierarchicalList* list = new HierarchicalList(name_, info.height, info.width, 0, 0);
  vector<ListItem*> roots;
  for (int i = 0; i < tasks_.size(); ++i) {
    roots.push_back((ListItem*) tasks_[i]);
  }
  list->SetDatasource(&roots);
  list->Draw();

  int ch;
  bool done = false;
  //while (!done && (ch = list->GetInput())) {
  while (!done && (ch = getch())) {
    switch (ch) {
      case 'a':
      case 'A':
        tasks_.push_back(new Task(DialogBox::RunMultiLine("Enter New Task",
            "",
            winwidth(win) / 3,
            winheight(win) / 3), ""));
        roots.push_back((ListItem*) tasks_[tasks_.size() - 1]);
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
        list->SelectItem(-1);
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

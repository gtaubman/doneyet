#include <ncurses.h>
#include <iostream>
#include <fstream>
#include "utils.h"
#include "dialog-box.h"
#include "workspace.h"
#include "file-manager.h"
#include "serializer.h"
#include "list-chooser.h"

Workspace::Workspace() :
  menubar_(NULL),
  project_(NULL),
  list_(NULL),
  done_(false) {

  // Figure out the layout of the screen:
  window_info info = CursesUtils::get_window_info(stdscr);
  const int notes_width = 40;
  string name = "";
  ColumnSpec spec("Task:X,N:1,Created:24,Completed:24", false);
  list_ = new HierarchicalList(name,
      info.height,
      info.width - notes_width,
      0,
      0,
      spec);

  ColumnSpec notes_spec("Notes:X", false);
  notes_list_ = new HierarchicalList(name,
      info.height,
      notes_width,
      0,
      info.width - notes_width,
      notes_spec);
  notes_list_->SetDatasource(&notes_source_);

  // Initialize the menu bar.
  InitializeMenuBar();

  // Figure out which project we want to load.
  FileManager* fm = FileManager::DefaultFileManager();
  if (fm->NumSavedProjects()) {
    string project_name = ListChooser::GetChoice(fm->SavedProjectNames());
    if (project_name.empty()) {
      Quit();
    } else {
      project_ = Project::NewProjectFromFile(fm->ProjectDir() + project_name);
    }
  } else {
    project_ = CreateNewProject();
  }

  // Unserialize our project.
  if (project_)
    list_->SetDatasource(project_);

  Run();
}

Workspace::~Workspace() {
  delete project_;
  delete menubar_;
  delete list_;
  delete notes_list_;
}

// A: Show all tasks.
// a: Add a task.
// M: Show menubar.
//  - j & k: Change menu item.
//  - l & h: Change menu.
//  - \r: Select menu item.
//  - Esc: Hide menubar.
// m: Move a task.
//  - u: Up.
//  - d: Down.
// n: Add note to selected task.
// v: View notes of selected task.
// j: Select next task.
// k: Select previous task.
// e: Edit selected task.
// d: Delete selected task.
// c: Toggle collapsed state of selected task.
// R: Show only uncompleted tasks.
// C: Show only tasks completed in the last week.
// f: Search tasks.
// Esc: Select no item.
// Spc: Toggle selected task status.
// Tab: Select next column.
// \r: Quit.
void Workspace::Run() {
  list_->Draw();
  notes_list_->Draw();
  doupdate();

  int ch;
  Task* selected_task;
  while (!done_ && (ch = getch())) {
    // Get the selected task in the list.
    selected_task = static_cast<Task*>(list_->SelectedItem());
    switch (ch) {
      case 'a':  // Add task
        AddTask(selected_task);
        break;
      case 'A':  // Show all tasks
        ShowAllTasks();
        break;
      case 'c':  // Toggle collapsed state
        list_->ToggleExpansionOfSelectedItem();
        break;
      case 'C':  // Show weekly completed tasks
        ShowTasksCompletedLastWeek();
        break;
      case 'd':  // Deleted selected task
        list_->SelectPrevItem();
        project_->DeleteTask(selected_task);
        PerformFullListUpdate();
        break;
      case 'e':  // Edit selected task
        list_->EditSelectedItem();
        break;
      case 'f':  // Filter on string
        RunFind();
        break;
      case KEY_DOWN:
      case 'j':  // Select next task
        list_->SelectNextItem();
        break;
      case KEY_UP:
      case 'k':  // Select prev task
        list_->SelectPrevItem();
        break;
      case 'm':  // Move task
        MoveTask(selected_task);
        break;
      case 'M':  // Show menu bar
        ShowMenuBar(selected_task);
        break;
      case 'n': // Add note to selected task
        AddNote(selected_task);
        break;
      case 'R':  // Show unfinished tasks
        ShowUnfinishedTasks();
        break;
      case 'v':
        ViewNotes(selected_task);
        break;
      case ' ':  // Toggle task status
        ToggleStatus(selected_task);
        break;
      case 27:  // Escape
        list_->SelectNoItem();
        break;
      case 'q':
        Quit();
        break;
    }
    DisplayNotes(static_cast<Task*>(list_->SelectedItem()));
    list_->Draw();
    notes_list_->Draw();
    doupdate();
  }
}

void Workspace::AddTask(Task* t) {
  // Get the task text:
  string text = DialogBox::RunMultiLine("Enter New Task",
                                        "",
                                        CursesUtils::winwidth() / 3,
                                        CursesUtils::winheight() / 3);
  if (text.empty())
    return;

  if (t == NULL) {
    // We add a task to the root level list if no task is selected.
    project_->AddTaskNamed(text);
  } else {
    // Add the task as a subtask of the selected task.
    t->AddSubTask(new Task(text, ""));
  }
  PerformFullListUpdate();
}

void Workspace::MoveTask(Task* t) {
  if (t == NULL || t->Parent() == NULL)
    return;

  // Now we get characters until they hit return.
  int amount_moved = 0;
  int ch;
  bool done = false;
  while (!done) {
    ch = getch();
    switch (ch) {
      case KEY_UP:
      case 'k':
      case 'u':
        t->MoveUp();
        ++amount_moved;
        break;
      case KEY_DOWN:
      case 'j':
      case 'd':
        t->MoveDown();
        --amount_moved;
        break;
      case 27:
        // We want to put the task where it started.
        while (amount_moved > 0) {
          t->MoveDown();
          project_->FilterTasks();
          --amount_moved;
        }
        while(amount_moved < 0) {
          t->MoveUp();
          project_->FilterTasks();
          ++amount_moved;
        }
        done = true;
        break;
      case '\r':
        done = true;
        break;
    }
    project_->FilterTasks();
    list_->Update();
    list_->Draw();
    doupdate();
  }
}

void Workspace::ShowMenuBar(Task* t) {
  string menu_answer;
  int ch;
  bool done = false;
  menubar_->ShowNextMenu();
  while (!done) {
    ch = getch();
    switch(ch) {
      case KEY_DOWN:
      case 'j':
        menubar_->SendEventToMenu(REQ_DOWN_ITEM);
        break;
      case KEY_UP:
      case 'k':
        menubar_->SendEventToMenu(REQ_UP_ITEM);
        break;
      case KEY_RIGHT:
      case 'l':
        list_->Draw();
        menubar_->ShowNextMenu();
        break;
      case KEY_LEFT:
      case 'h':
        list_->Draw();
        menubar_->ShowPreviousMenu();
        break;
      case '\r':
        menu_answer = menubar_->SelectedItem();
        done = true;
        break;
      case 27:  // escape
        menu_answer.clear();
        done = true;
        break;
    }
    doupdate();
  }

  HandleMenuInput(menu_answer);
}

void Workspace::AddNote(Task* t) {
  if (t == NULL)
    return;
  string note = DialogBox::RunCenteredWithWidth("Add Note",
                                                "",
                                                CursesUtils::winwidth() / 3);
  if (!note.empty())
    t->AddNote(note);
}

void Workspace::ViewNotes(Task* t) {
  if (t != NULL) {
    ListChooser::GetChoice(t->Notes());
  }
}

void Workspace::HandleMenuInput(const string& input) {
  if (input.empty()) {
    return;
  } else if (input == "New") {
    NewProject();
  } else if (input == "Open") {
    OpenProject();
  } else if (input == "Save") {
    SaveCurrentProject();
  } else if (input == "Quit") {
    SaveCurrentProject();
    Quit();
  } else if (input == "All Tasks") {
    ShowAllTasks();
  } else if (input == "Completed Tasks") {
    ShowTasksCompletedLastWeek();
  } else if (input == "Incomplete Tasks") {
    ShowUnfinishedTasks();
  } else if (input == "Find...") {
    RunFind();
    list_->ScrollToTop();
  } else if (input == "Plain Text") {
    const char* path = "/tmp/snippet.txt";
    std::ofstream out(path, std::ios::out);
    if (out.fail()) {
      beep();
    } else {
      out << (*project_);
      endwin();
      system("less /tmp/snippet.txt");
      list_->Draw();
      doupdate();
    }
  }
  list_->ScrollToTop();
}

void Workspace::RunFind() {
  string tmp_str = DialogBox::RunCenteredWithWidth("Enter Search Term:",
      "",
      CursesUtils::winwidth(stdscr) / 3);
  if (!tmp_str.empty()) {
    project_->RunSearchFilter(tmp_str);
    list_->Update();
    list_->ScrollToTop();
  }
}

void Workspace::ToggleStatus(Task* t) {
  if (t != NULL && !t->NumChildren()) {
    switch (t->Status()) {
      case CREATED:
        t->SetStatus(IN_PROGRESS);
        break;
      case IN_PROGRESS:
        t->SetStatus(COMPLETED);
        break;
      case COMPLETED:
        t->SetStatus(PAUSED);
        break;
      case PAUSED:
        t->SetStatus(CREATED);
        break;
    }
    project_->RecomputeNodeStatus();
  }
}

void Workspace::NewProject() {
  SaveCurrentProject();
  Project* p = CreateNewProject();
  if (p != NULL) {
    delete project_;
    project_ = p;
    list_->SetDatasource(project_);
    project_->ShowAllTasks();
    list_->Update();
  }
}

void Workspace::OpenProject() {
  SaveCurrentProject();
  string new_project = ListChooser::GetChoice(fm->SavedProjectNames());
  if (!new_project.empty()) {
    delete project_;
    project_ = Project::NewProjectFromFile(fm->ProjectDir() + new_project);
    list_->SetDatasource(project_);
    project_->ShowAllTasks();
    list_->Update();
  }
}

void Workspace::SaveCurrentProject() {
  // Serialize the current project to its file.
  if (project_ == NULL) {
    return;
  }

  string filename = FileManager::DefaultFileManager()->ProjectDir() +
    project_->Name();
  Serializer s("", filename);
  project_->Serialize(&s);
  s.CloseAll();
}

void Workspace::PerformFullListUpdate() {
  project_->RecomputeNodeStatus();
  project_->FilterTasks();
  list_->Update();
}

void Workspace::DisplayNotes(Task* t) {
  if (t == NULL) {
    notes_source_.Clear();
  } else {
    notes_source_.SetVector(t->Notes());
  }
}

void Workspace::InitializeMenuBar() {
  menubar_ = new MenuBar();
  Menu* m = menubar_->AddMenu("Project");
  m->AddMenuItem("New");
  m->AddMenuItem("Open");
  m->AddMenuItem("Save");
  m->AddMenuItem("Quit");

  m = menubar_->AddMenu("View");
  m->AddMenuItem("Find...");
  m->AddMenuItem("All Tasks");
  m->AddMenuItem("Completed Tasks");
  m->AddMenuItem("Incomplete Tasks");

  m = menubar_->AddMenu("Generate");
  m->AddMenuItem("Plain Text");
}

void Workspace::Quit() {
  SaveCurrentProject();
  done_ = true;
}

Project* Workspace::CreateNewProject() {
  string answer = DialogBox::RunCentered(
      "Please Enter a Project Name",
      "Default Project");
  if (!answer.empty()) {
    return new Project(answer);
  }
  return NULL;
}

void Workspace::ShowAllTasks() {
  project_->ShowAllTasks();
  list_->Update();
  list_->ScrollToTop();
}

void Workspace::ShowTasksCompletedLastWeek() {
  project_->ShowCompletedLastWeek();
  list_->Update();
  list_->ScrollToTop();
}

void Workspace::ShowUnfinishedTasks() {
  project_->ArchiveCompletedTasks();
  list_->Update();
  list_->ScrollToTop();
}

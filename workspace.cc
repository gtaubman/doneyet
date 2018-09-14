#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "constants.h"
#include "utils.h"
#include "dialog-box.h"
#include "doneyet-config.h"
#include "workspace.h"
#include "file-manager.h"
#include "file-versions.h"
#include "serializer.h"
#include "list-chooser.h"

static bool do_resize;

Workspace::Workspace() :
  menubar_(NULL),
  project_(NULL),
  list_(NULL),
  notes_list_(NULL),
  done_(false) {

  // Initialize the menu bar.
  InitializeMenuBar();

  // Figure out which project we want to load.
  FileManager* fm = FileManager::DefaultFileManager();
  if (fm->NumSavedProjects()) {
    string project_name = ListChooser::GetChoice(fm->SavedProjectNames());
    if (project_name.empty()) {
      return;
    } else {
      project_ = Project::NewProjectFromFile(fm->ProjectDir() + project_name);
    }
  } else {
    project_ = CreateNewProject();
    if (project_ == NULL) {
      return;
    }
  }

  InitializeLists();

  // Unserialize our project.
  if (project_)
    list_->SetDatasource(project_);

  // Enable catching sigwinch.
  struct sigaction sa;
  sa.sa_sigaction = SigWinchHandler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGWINCH, &sa, NULL);

  Run();
}

Workspace::~Workspace() {
  delete project_;
  delete menubar_;
  delete list_;
  if (notes_list_ != NULL) {
	  delete notes_list_;
  }
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
  if (notes_list_ != NULL) {
    notes_list_->Draw();
  }
  doupdate();

  int ch;
  Task* selected_task;
  while (!done_ && (ch = getch())) {
    if (do_resize) {
      delete list_;
      delete notes_list_;
      list_ = NULL;
      notes_list_ = NULL;

      // End our window and refresh to get the new terminal size.
      endwin();
      refresh();

      // Construct our new lists.
      InitializeLists();

      doupdate();
      do_resize = false;
      continue;
    }
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
      case 'd': { // Deleted selected task
        // If nothing is selected there's nothing to delete.
        if (selected_task == NULL) {
          break;
        }

        // First things first, potentially make sure they really want to delete
        // this task.
        DoneyetConfig* config = DoneyetConfig::GlobalConfig();
        assert(config != NULL);
        if (config->PromptOnDeleteTask()) {
          if (!ListChooser::GetYesNo("Are you sure you want to delete this task?",
                                     false)) {
            break;
          }
        }

        bool first_task_selected = selected_task == project_->Root(0);

        list_->SelectPrevItem();
        project_->DeleteTask(selected_task);
        PerformFullListUpdate();
        if (first_task_selected) {
          // If the top task was the one that we deleted, then the
          // list_->SelectPrevItem() won't have actually done anything, leaving
          // the list with the deleted item still selected.  So, we manually
          // select the new top task.
          list_->ScrollToTop();
        }

        // Also, this could have been the last task.  If it was, make sure no
        // task is selected now.
        if (project_->NumTasks() == 0) {
          list_->SelectNoItem();
        }
        break;
      }
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
      case 'S':  // Save project.
        SaveCurrentProject();
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
    if (notes_list_ != NULL) {
      notes_list_->Draw();
    }
    doupdate();
  }
}

void Workspace::SigWinchHandler(int signal,
                                siginfo_t* siginfo,
                                void* signal_ucontext) {
  do_resize = true;
}

void Workspace::InitializeLists() {
  window_info info = CursesUtils::get_window_info(stdscr);

  // If the window is too small, don't show the notes panel.
  int notes_width = info.width < Constants::kMinWidthForNotesView ?
      0 : Constants::kNoteViewSize;

  string name = "";
  ColumnSpec spec("Task:X,N:1,Created:24,Completed:24", false);
  list_ = new HierarchicalList(name,
                               info.height,
                               info.width - notes_width,
                               0,
                               0,
                               spec);
  list_->SetDatasource(project_);

  if (notes_width) {
    ColumnSpec notes_spec("Notes:X", false);
    notes_list_ = new HierarchicalList(name,
                                       info.height,
                                       notes_width,
                                       0,
                                       info.width - notes_width,
                                       notes_spec);
    notes_list_->SetDatasource(&notes_source_);
  } else {
    notes_list_ = NULL;
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
    if (t->HasNotes() ) {
      string selected_note = ListChooser::GetMappedChoice(t->MappedNotes()); // use MappedNotes here
      if (!selected_note.empty() ) { //user selected item (Abort with ESC)
        string answer = DialogBox::RunMultiLine("Please Edit Note",
            selected_note,
            CursesUtils::winwidth() / 3,
            CursesUtils::winheight() / 3);
        if (answer.empty() ) {
          t->DeleteNote(selected_note);
        }
        else if (answer.compare(selected_note) != 0) {
          //note altered
          t->DeleteNote(selected_note);
          t->AddNote(answer);
        }
        //else nothing changed
      }
    }
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
      case NUM_STATUSES:  // Here to appease the compiler.
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
  s.SetVersion(NOTES_VERSION);
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

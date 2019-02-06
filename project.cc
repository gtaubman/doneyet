#include "project.h"
#include <map>
#include "hierarchical-list.h"
#include "utils.h"

using std::map;

Project::Project(string name)
    : name_(name) {
    ShowAllTasks();
}

Project::~Project() {
    for (int i = 0; i < tasks_.size(); ++i) {
        tasks_[i]->Delete();
    }
}

void Project::FilterTasks(FilterPredicate<Task>* filter) {
    // Filter all the children of the root tasks.
    for (int i = 0; i < tasks_.size(); ++i) {
        tasks_[i]->ApplyFilter(filter);
    }

    // Finally filter the root tasks themselves.
    filtered_tasks_ = filter->FilterVector(tasks_);
}

Task* Project::AddTaskNamed(const string& name) {
    Task* nt = new Task(name, "");
    tasks_.push_back(nt);
    return nt;
}

void Project::Serialize(Serializer* s) {
    // Write a serialization version.
    s->WriteInt64(s->Version());

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
    Serializer s(path, "");
    if (!s.Okay()) {
        return NULL;
    }

    // Read the file version
    uint64 file_version = s.ReadUint64();
    s.SetVersion(file_version);

    // Find the project's name
    string project_name = s.ReadString();
    Project* p = new Project(project_name);

    // Find how many tasks there are.
    int num_tasks = s.ReadInt32();

    // First read in every task in the file.
    int task_identifier;
    int parent_pointer;
    map<int, Task*> task_map;
    map<Task*, int> tasks_parents;
    vector<Task*> tasks;
    for (int i = 0; i < num_tasks; ++i) {
        // Read in the values.
        task_identifier = s.ReadUint64();
        Task* t = Task::NewTaskFromSerializer(&s);
        parent_pointer = s.ReadUint64();

        tasks_parents[t] = parent_pointer;
        task_map[task_identifier] = t;
        tasks.push_back(t);
    }

    // Then re-assemble the tree structure.
    for (int i = 0; i < tasks.size(); ++i) {
        Task* t = tasks[i];
        if (tasks_parents[t] == 0) {
            // We have a root task.  Add it to the root list.
            p->tasks_.push_back(tasks[i]);
        } else {
            // We have a child task.  Add it to its parent's list.
            task_map[tasks_parents[t]]->AddSubTask(tasks[i]);
        }
    }

    p->ShowAllTasks();
    return p;
}

int Project::NumTasks() {
    int total = 0;
    for (int i = 0; i < tasks_.size(); ++i) {
        total += 1 + tasks_[i]->NumOffspring();
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

// Compute the status of all nodes.  Nodes which have children have their status
// for them (hence the need for this function).  A node with any IN_PROGRESS
// child is itself IN_PROGRESS.  If all of a node's children are PAUSED, the
// node is PAUSED.
void Project::RecomputeNodeStatus() {
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

int Project::NumFilteredRoots() {
    return filtered_tasks_.size();
}

Task* Project::FilteredRoot(int r) {
    return filtered_tasks_[r];
}

void Project::ShowAllTasks() {
    GTFilterPredicate<Task, time_t>* gtfp =
        new GTFilterPredicate<Task, time_t>(-1, Task::CompletionDateWrapper);
    base_filter_.Clear();
    base_filter_.AddChild(gtfp);
    FilterTasks();
}

void Project::ArchiveCompletedTasks() {
    EqualityFilterPredicate<Task, TaskStatus>* efp =
        new EqualityFilterPredicate<Task, TaskStatus>(COMPLETED, Task::StatusWrapper);
    efp->SetIsNot(true);
    base_filter_.Clear();
    base_filter_.AddChild(efp);
    FilterTasks();
}

void Project::ShowCompletedLastWeek() {
    // We check if their completion date is > one week ago.
    time_t week_ago;
    std::time(&week_ago);
    week_ago -= 60 * 60 * 24 * 7;
    GTFilterPredicate<Task, time_t>* gtfp =
        new GTFilterPredicate<Task, time_t>(week_ago, Task::CompletionDateWrapper);
    GTFilterPredicate<Task, int>* has_filtered_child =
        new GTFilterPredicate<Task, int>(0, Task::NumFilteredOffspringWrapper);
    OrFilterPredicate<Task>* or_filter = new OrFilterPredicate<Task>();
    or_filter->AddChild(gtfp);
    or_filter->AddChild(has_filtered_child);
    base_filter_.Clear();
    base_filter_.AddChild(or_filter);
    FilterTasks();
}

void Project::RunSearchFilter(const string& needle) {
    // Clear the current filters.
    base_filter_.Clear();

    // Create the search filters.  Right now it only searches on the text and
    // description of the tasks.
    StringContainsFilterPredicate<Task>* title_filter =
        new StringContainsFilterPredicate<Task>(needle, Task::TitleWrapper);

    StringContainsFilterPredicate<Task>* description_filter =
        new StringContainsFilterPredicate<Task>(needle, Task::DescriptionWrapper);

    GTFilterPredicate<Task, int>* has_filtered_child =
        new GTFilterPredicate<Task, int>(0, Task::NumFilteredOffspringWrapper);

    OrFilterPredicate<Task>* or_filter = new OrFilterPredicate<Task>();
    or_filter->AddChild(title_filter);
    or_filter->AddChild(description_filter);
    or_filter->AddChild(has_filtered_child);

    base_filter_.AddChild(or_filter);
    FilterTasks();
}

ostream& operator <<(ostream& out, Project& project) {
    for (int i = 0; i < project.NumFilteredRoots(); ++i) {
        project.FilteredRoot(i)->ToStream(out, 0);
    }
    return out;
}

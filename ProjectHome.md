Doneyet is an ncurses based hierarchical todo list manager written in C++.  If you give it a whirl let me know!  I'd be happy to hear who's using it and any feature requests!  Mostly, I use it for:
  * Keeping track of things you need to do.
  * Tracking start and finish times and dates for tasks.
  * Generating lists of work completed in a given time frame.
  * Tons of other stuff!

## Information ##
DoneYet is a filter-based todo list manager.  Currently there are only four filters but in future versions there will be more and they will be user editable.  Currently the filters consist of:
  * **Show All Tasks** - This shows all tasks in the current project.
  * **Show Unfinished Tasks** - This shows any task with a status of _unstarted_, _in progress_, or _paused_.
  * **Show Completed Tasks** - This shows only tasks that have a completion date within 7 days of now.
  * **Find** - This filter takes a user specified string and shows any that match.  This uses case-sensitive search.

### Saving ###
Doneyet will save on quit, or when choosing 'Save' from the 'Project' menu.

### Key Shortcuts ###
Doneyet is used primarily through key commands.  There is a menu system in place but not everything can be achieved through it.  The key commands are as follows:
  * **A** - Apply the _Show All Tasks_ filter.
  * **a** - If a task is selected, add a new subtask of that task.  If no task is selected add a root level task.
  * **M** - Show the menu bar.
    * **j** and **k** - Change menu item.
    * **l** and **h** - Change menu.
    * **Return** - Select the selected menu item.
    * **Escape** - Hide the menu bar.
  * **m** - Move the currently selected task. **Note this doesn't work for root level tasks yet.**
    * **k**/**u**/**Up Arrow** - Move selected task up.
    * **j**/**d**/**Down Arrow** - Move selected task down.
    * **Return** - Place task at current position.
    * **Escape** - Place task to where it was originally.
  * **n** - Add a note to the selected task.
  * **v** - View the notes of the selected task.
  * **j** - Selected next task.
  * **k** - Select previous task.
  * **Escape** - Select no task.
  * **e** - Edit selected task.
  * **d** - Delete selected task.
  * **c** - Toggle collapsed state of selected task.
  * **R** - Apply the _Show Uncompleted Tasks_ filter.
  * **C** - Apply the _Show Completed Tasks_ filter.
  * **f** - Apply the _Find Tasks_ filter.
  * **S** - Save the project.
  * **Space** - Toggle the status of the selected item.  White is unstarted, green is in progress, blue is completed and red is paused.
  * **q** - Quit.

### Menu System ###
The menu system also contains the 'Plain Text' menu item in the 'Generate' menu.  This creates a text file of whatever is currently filtered in `/tmp/snippet.txt` and then calls `less` on that file.  A potential use of this is reporting weekly progress in email.  Copying directly out of the ncurses window is difficult and time consuming.  This however generates a plain text version with no borders.

### Screenshots ###
![http://www.gabetaubman.com/external_images/doneyet1.png](http://www.gabetaubman.com/external_images/doneyet1.png)
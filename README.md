# Doneyet
![Screenshot](http://www.gabetaubman.com/external_images/doneyet1.png)

Doneyet is an ncurses based hierarchical todo list manager written in C++. If you give it a whirl let me know! I'd be happy to hear who's using it and any feature requests! Mostly, I use it for:

* Keeping track of things you need to do.
* Tracking start and finish times and dates for tasks.
* Generating lists of work completed in a given time frame.
* Tons of other stuff!

## Information
DoneYet? is a filter-based todo list manager. Currently there are only four filters but in future versions there will be more and they will be user editable. Currently the filters consist of:

* Show All Tasks - This shows all tasks in the current project.
* Show Unfinished Tasks - This shows any task with a status of unstarted, in progress, or paused.
* Show Completed Tasks - This shows only tasks that have a completion date within 7 days of now.
* Find - This filter takes a user specified string and shows any that match. This uses case-sensitive search.

# Saving
Doneyet will save on quit, or when choosing 'Save' from the 'Project' menu.

# Key Shortcuts
Doneyet is used primarily through key commands. There is a menu system in place but not everything can be achieved through it. The key commands are as follows:

* A - Apply the Show All Tasks filter.
* a - Create a new task (or a subtask if pressed while a task is selected).
* M - Show the menu bar.
  * j and k - Change menu item.
  * l and h - Change menu.
  * Return - Select the selected menu item.
  * Escape - Hide the menu bar.
* m - Move the currently selected task. Note this doesn't work for root level tasks yet.
  * k/u/Up Arrow - Move selected task up.
  * j/d/Down Arrow - Move selected task down.
  * Return - Place task at current position.
  * Escape - Place task to where it was originally.
* n - Add a note to the selected task.
* v - View the notes of the selected task.
* j - Selected next task.
* k - Select previous task.
* Escape - Select no task.
* e - Edit selected task.
* d - Delete selected task.
* c - Toggle collapsed state of selected task.
* R - Apply the Show Uncompleted Tasks filter.
* C - Apply the Show Completed Tasks filter.
* f - Apply the Find Tasks filter.
* S - Save the project.
* Space - Toggle the status of the selected item. White is unstarted, green is in progress, blue is completed and red is paused.
* h - Shows a help dialog.
* q - Quit.

# Menu System
The menu system also contains the 'Plain Text' menu item in the 'Generate' menu. This creates a text file of whatever is currently filtered in /tmp/snippet.txt and then calls less on that file. A potential use of this is reporting weekly progress in email. Copying directly out of the ncurses window is difficult and time consuming. This however generates a plain text version with no borders.

# Versioning and Features

## doneyet-1.1.0 - edit and delete of notes & help dialog
Features:
* added deleting and editing of notes
* drawing closing T-junctions on column ends
* displaying a help dialog upon pressing 'h', showing all current shortcuts

Bugfixes:
* segmentation fault (ESC twice at startup)
* removing unnecessary saving (is already performed by Quit() )

## doneyet-1.0.0 - original version from google code import

# Compilation/Installation

## Linux & most other Unix-like Operating Systems
Clone or download doneyet, cd into the main source code repository and then run make.
```
git clone git@github.com:gtaubman/doneyet.git
cd doneyet
make
```
The doneyet binary can now be copied to whatever system path suits best for you. You may also choose to run it right from here.
```
./doneyet
```

## Mac OS X
### Versions before macOS Mojave 10.14
Please follow the Linux instructions.

### Version macOS Mojave 10.14
There seems to be a bug in the way linking against the system supplied libcurses is done in macOS 10.14 when combined with libmenu. Please install homebrew, instructions are available on https://brew.sh/. Then install ncurses
```
brew install ncurses
```
and follow the post-installation instructions by adding the pkg-config definition of brew-installed ncurses to your pkg-config search path
```
echo 'export PKG_CONFIG_PATH="/usr/local/opt/ncurses/lib/pkgconfig"' >> ~/.bash_profile
```
you should ensure that the .bash_profile is sourced (close Terminal.app window and open up a new one if you don't know what that means)
Please follow the Linux instructions now.

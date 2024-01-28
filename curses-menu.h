#ifndef CURSES_MENU_H_
#define CURSES_MENU_H_

#include <ncursesw/menu.h>

#include <string>
#include <vector>

#include "utils.h"

using std::string;
using std::vector;

class Menu {
 public:
  Menu(const string& name, int xloc)
      : xloc_(xloc),
        name_(name),
        fg_color_(COLOR_PAIR(8) | A_BOLD),
        bg_color_(COLOR_PAIR(9)),
        displaying_(false){};
  ~Menu(){};

  void AddMenuItem(const string& item);
  void Display();
  string SelectedItem();
  void StopDisplaying();

  int Location() const { return xloc_; }
  string Name() const { return name_; }
  MENU* MenuObject() { return menu_; }

 private:
  ITEM** items_;
  MENU* menu_;
  WINDOW* frill_window_;
  WINDOW* text_window_;
  vector<string> item_strings_;
  int xloc_;
  string name_;
  int fg_color_;
  int bg_color_;
  bool displaying_;
};

class MenuBar {
 public:
  MenuBar();
  ~MenuBar();

  void DrawMenuBar();
  Menu* AddMenu(const string& menu_name);
  void SendEventToMenu(int c);
  void ShowNextMenu();
  void ShowPreviousMenu();
  string SelectedItem();

 private:
  int space_between_menus_;
  vector<Menu*> menus_;
  int current_menu_;
  int current_loc_;
};

#endif  // CURSES_MENU_H_

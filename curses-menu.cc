#include "curses-menu.h"
#include <stdlib.h>

Menu::Menu(const string& name, int xloc) {
  name_ = name;
  xloc_ = xloc;
  fg_color_ = COLOR_PAIR(8) | A_BOLD;
  bg_color_ = COLOR_PAIR(9);
  displaying_ = false;
}

Menu::~Menu() {}

void Menu::AddMenuItem(const string& item) { item_strings_.push_back(item); }

void Menu::Display() {
  vector<string> choices = item_strings_;
  bool border = true;

  if (!displaying_) {
    // Create the menu and its items:
    items_ = (ITEM**)malloc((1 + choices.size()) * sizeof(ITEM*));
    items_[choices.size()] = NULL;
    for (int i = 0; i < choices.size(); ++i) {
      items_[i] = new_item(choices[i].c_str(), "");
    }

    // Create the menu itself:
    menu_ = new_menu(items_);
    set_menu_fore(menu_, fg_color_);
    set_menu_back(menu_, bg_color_);

    // Create the windows to hold it:
    int rows, cols;
    scale_menu(menu_, &rows, &cols);
    if (rows > CursesUtils::winheight(stdscr)) {
      rows = CursesUtils::winheight(stdscr) - 4 - 2 * border;
    }
    if (cols > CursesUtils::winwidth(stdscr)) {
      cols = CursesUtils::winwidth(stdscr) - 4 - 2 * border;
    }

    // Figure out the size of the window depending on whether or not we're
    // drawing a border:
    int spacing = 0;
    if (border) {
      spacing += 2;
    }

    frill_window_ = newwin(rows + spacing, cols + spacing, 1, Location());
    keypad(frill_window_, true);
    text_window_ = derwin(frill_window_, rows, cols, border, border);
    set_menu_win(menu_, frill_window_);
    set_menu_sub(menu_, text_window_);

    // Set the mark for the menu:
    set_menu_mark(menu_, " ");
  }
  displaying_ = true;

  // Begin drawing
  box(frill_window_, 0, 0);
  refresh();
  post_menu(menu_);
  wrefresh(frill_window_);
}

string Menu::SelectedItem() {
  return item_strings_[item_index(current_item(menu_))];
}

void Menu::StopDisplaying() {
  if (!displaying_) {
    return;
  }

  unpost_menu(menu_);
  for (int i = 0; i < item_strings_.size(); ++i) {
    free_item(items_[i]);
  }
  free_menu(menu_);
  delwin(text_window_);
  delwin(frill_window_);
  free(items_);
  displaying_ = false;
}

MenuBar::MenuBar() {
  current_menu_ = -1;
  space_between_menus_ = 1;
  current_loc_ = 0;
}

MenuBar::~MenuBar() {
  for (int i = 0; i < menus_.size(); ++i) {
    delete (menus_[i]);
  }
}

void MenuBar::DrawMenuBar() {
  // First cover the top with color so nothing shows through
  attron(COLOR_PAIR(3) | A_REVERSE);
  move(0, 0);
  for (int i = 0; i < CursesUtils::winwidth(stdscr); ++i) {
    addch(' ');
  }

  // Now draw the menu names:
  move(0, 0);
  int x = 0;
  for (int i = 0; i < menus_.size(); ++i) {
    if (i == current_menu_) {
      attron(A_BOLD);
    }
    mvprintw(0, x, "%s", menus_[i]->Name().c_str());
    x += menus_[i]->Name().length() + 1;
    if (i == current_menu_) {
      attroff(A_BOLD);
    }
  }
  attroff(COLOR_PAIR(3) | A_REVERSE);
}

Menu* MenuBar::AddMenu(const string& menu_name) {
  Menu* m = new Menu(menu_name, current_loc_);
  menus_.push_back(m);
  current_loc_ += menu_name.length() + space_between_menus_;
  return m;
}

void MenuBar::ShowCurrentMenu() {
  DrawMenuBar();
  menus_[current_menu_]->Display();
}

void MenuBar::HideCurrentMenu() { menus_[current_menu_]->StopDisplaying(); }

void MenuBar::SendEventToMenu(int c) {
  menu_driver(menus_[current_menu_]->MenuObject(), c);
  DrawMenuBar();
  menus_[current_menu_]->Display();
}

void MenuBar::ShowNextMenu() {
  if (current_menu_ == menus_.size() - 1) {
    menus_[current_menu_]->Display();
    DrawMenuBar();
    return;
  }

  if (current_menu_ >= 0) menus_[current_menu_]->StopDisplaying();
  ++current_menu_;
  DrawMenuBar();
  menus_[current_menu_]->Display();
}

void MenuBar::ShowPreviousMenu() {
  if (current_menu_ == 0) {
    menus_[current_menu_]->Display();
    DrawMenuBar();
    return;
  }

  menus_[current_menu_]->StopDisplaying();
  --current_menu_;
  DrawMenuBar();
  menus_[current_menu_]->Display();
}

string MenuBar::SelectedItem() { return menus_[current_menu_]->SelectedItem(); }

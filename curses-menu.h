#ifndef __CURSES_MENU__
#define __CURSES_MENU__

#include <menu.h>
#include <string>
#include <vector>
#include "utils.h"

using std::string;
using std::vector;

class Menu {
  public:
    Menu(const string& name, int xloc);
    ~Menu();

    void AddMenuItem(const string& item);
    void Display();
    string SelectedItem();
    void StopDisplaying();

    vector<string> Items() {
        return item_strings_;
    }
    int Location() {
        return xloc_;
    }
    string Name() {
        return name_;
    }
    MENU* MenuObject() {
        return menu_;
    }

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
    void ShowCurrentMenu();
    void HideCurrentMenu();
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

#endif

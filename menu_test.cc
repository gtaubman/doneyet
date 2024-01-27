#include <locale.h>
#include <ncurses/menu.h>
#include <stdlib.h>

ITEM **it;
MENU *me;

void quit(void)
{
  int i;

  unpost_menu(me);
  free_menu(me);

  for(i=0; i<=4; i++)
  {
    free_item(it[i]);
  }

  free(it);
  endwin();
}

int main(void)
{
  setlocale(LC_ALL, "");
  int ch;

  initscr();
  atexit(quit);
  clear();
  noecho();
  curs_set(0);
  cbreak();
  nl();
  keypad(stdscr, TRUE);

  it = (ITEM **)calloc(5, sizeof(ITEM *));
  it[0] = new_item("M1", "");
  it[1] = new_item("M2ö", "");
  it[2] = new_item("M3", "");
  it[3] = new_item("Ende", "");
  it[4] = 0;
  me = new_menu(it);
  post_menu(me);	

  mvaddstr(7, 3, "Programm mittels Menü oder F1-Funktionstaste beenden");
  refresh();

  while((ch=getch()) != KEY_F(1))
  {
    switch(ch)
    {
      case KEY_DOWN:
        menu_driver(me, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(me, REQ_UP_ITEM);
        break;
      case 0xA: /* Return- bzw. Enter-Taste -> ASCII-Code */
        if(item_index(current_item(me)) == 3)
          exit(0);	
    }
  } 

  return (0);  
}

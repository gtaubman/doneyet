#include <locale.h>
#include <ncurses/form.h>
#include <stdlib.h>

FIELD  **fi;
FORM   *fo;

void quit(void)
{
  int i;
  unpost_form(fo);
  free_form(fo);
  for(i=0; i<=3; i++)
  {
    free_field(fi[i]);
  }
  free(fi);
  endwin();
}

int main(void)
{
  setlocale(LC_ALL, "");

  initscr();
  atexit(quit);
  clear();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  start_color();
 
  init_pair(1, COLOR_YELLOW, COLOR_BLUE);
  init_pair(2, COLOR_BLACK, COLOR_WHITE);
 
  bkgd(COLOR_PAIR(1));

  fi = (FIELD **)calloc(4, sizeof(FIELD *));
  fi[0] = new_field(1, 10, 2, 3, 0, 0);
  fi[1] = new_field(1, 10, 2, 18, 0, 0);
  fi[2] = new_field(1, 15, 2, 33, 0, 0);
  fi[3] = 0;
 
  for(int i=0; i<3; i++)
  {
    set_field_fore(fi[i], COLOR_PAIR(2));
    set_field_back(fi[i], COLOR_PAIR(2));
  }
 
  fo = new_form(fi);
  post_form(fo);        

  mvaddstr(2, 15, "+");
  mvaddstr(2, 30, "=");
  mvaddstr(5, 3, "terminate with F1");
  mvaddstr(6, 3, "öäüfiancé");

    refresh();

  int ch;
  while((getch()==OK))
  {
    switch(ch)
    {
      case KEY_RIGHT:
        form_driver(fo, REQ_NEXT_FIELD);
        break;
      case KEY_LEFT:
        form_driver(fo, REQ_PREV_FIELD);
        break;
      default:
        form_driver(fo, ch);
    }
  }   

  return (0);  
}

#include "project.h"

static int entryCB (EObjectType cdktype,
                     void *object,
                     void *clientData, chtype key) {
  CDKBUTTONBOX* buttonbox = (CDKBUTTONBOX*) clientData;
  injectCDKButtonbox(buttonbox, key);
  return 1;
}

Project::Project(string name)
  : name_(name) {

}

Project::~Project() {

}

Project* Project::NewProject(CDKSCREEN* screen) {
  // Create an entry box for the project name.
  CDKENTRY* entry = newCDKEntry(screen,
      CENTER,
      CENTER,
      "<C>Create New Project",
      "Project Name: ",
      A_NORMAL,
      '.',
      vMIXED,
      40,
      0,
      256,
      TRUE,
      FALSE);


  // Create a button widget for them to back out of their action.
  char* buttons[] = {" OK ", " Cancel "};
  CDKBUTTONBOX* button_box = newCDKButtonbox(screen,
      getbegx(entry->win),
      getbegy(entry->win) + entry->boxHeight - 1,
      1,
      entry->boxWidth - 1,
      0, 1, 2,
      buttons, 2, A_REVERSE,
      true, false);

  // Make the two objects look nice together:
  setCDKEntryLLChar(entry, ACS_LTEE);
  setCDKEntryLRChar(entry, ACS_RTEE);
  setCDKButtonboxULChar(button_box, ACS_LTEE);
  setCDKButtonboxURChar(button_box, ACS_RTEE);

  // Set up some nice bindings.  We want up and down to go between the two
  // objects.
  bindCDKObject(vENTRY,
      entry,
      KEY_TAB,
      entryCB,
      button_box);

  drawCDKButtonbox(button_box, true);

  char* name = copyChar(activateCDKEntry(entry, NULL));
  int which_button = button_box->currentButton;

  if (which_button == 1 || entry->exitType != vNORMAL) {
    destroyCDKButtonbox(button_box);
    destroyCDKEntry(entry);
    return NULL;
  }

  destroyCDKButtonbox(button_box);
  destroyCDKEntry(entry);
  return new Project(name);
}

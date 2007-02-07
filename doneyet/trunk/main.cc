#include "cdk/cdk.h"
#include "project.h"

int main(int argc, char** argv) {
  CDK_PARAMS params;
  CDKparseParams(argc, argv, &params, "w:h:l:" CDK_MIN_PARAMS);

  // Set up CDK
  WINDOW* cursesWin = initscr();
  CDKSCREEN* cdkscreen = initCDKScreen(cursesWin);

  // Start CDK Colors.
  initCDKColor();

  Project* p = Project::NewProject(cdkscreen);
  refreshCDKScreen(cdkscreen);
  /* Set what ever was given from the command line. */
  //char* ep = copyChar(activateCDKEntry(entry, NULL));
  destroyCDKScreen(cdkscreen);
  endCDK();
  if (p) {
    printf("Project Created: %s\n", p->name_.c_str());
  } else {
    printf(" They chose to abort.\n");
  }
  return 0;
}

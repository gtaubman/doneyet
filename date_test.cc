#include "date.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

static int errors = 0;

ostream& ERROR() {
  ++errors;
  cout << "Error: ";
  return cout;
}

bool TestToString() {
  bool success = true;

  Date d;
  d.SetToNow();
  string str = d.ToString();
  int good_strlen = 24;
  cout << "\"" << str << "\"" << endl;

  if (str.length() > good_strlen) {
    success = false;
    ERROR() << "Date's stl string's length was > " << good_strlen << " ("
            << str.length() << ")" << endl;
  }

  if (strlen(str.c_str()) > good_strlen) {
    success = false;
    ERROR() << "Date's char* length was > " << good_strlen << " ("
            << strlen(str.c_str()) << ")" << endl;
  }

  return success;
}

int main() {
  bool s = TestToString();
  cout << errors << " errors." << endl;
  return s;
}

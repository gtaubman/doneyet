#include "serializer.h"

#include <locale.h>

#include <string>
using std::string;

int main() {
  setlocale(LC_ALL, "");
  // test data
  int mynum = 42;
  string mystring = "input";

  // create serializer in write-only configuration
  Serializer* s = new Serializer("", "./bla");

  // write test data
  s->WriteUint64(mynum);
  s->WriteString(mystring);
  s->CloseAll();
  delete s;

  // create serializer in read & write configuration, rereading dataset written
  // with serializer above
  s = new Serializer("./bla", "./bla2");

  // Read number from serialized data
  int compnum = s->ReadUint64();
  // compare, throw errors if different from expected
  if (compnum != mynum) {
    printf("TEST FAIL: Serializer : the numbers do not match\n");
  } else {
    printf("TEST SUCCESS: Serializer : numbers match\n");
  }
  // Read string from serialized data
  string compstring = s->ReadString();
  // compare, throw errors if different
  if (compstring != mystring) {
    printf("TEST FAIL: Serializer : the strings do not match\n");
  } else {
    printf("TEST SUCCESS: Serializer : strings match\n");
  }
  printf("sizeof int: %lu\n", sizeof(int));
  printf("sizeof time_t: %lu\n", sizeof(time_t));
  // cleanup on serializer
  s->CloseAll();
  delete s;
  return 0;
}


#include <assert.h>
#include <vector>
#include <iostream>
#include <string>
#include "filter-predicate.h"

using namespace std;

static int errors = 0;

void ERROR(const string& error) {
  cout << "ERROR: " <<  error << endl;;
  ++errors;
}

class TestObj {
 public:
  TestObj(int val) {
    val_ = val;
  }

  int Val() { return val_; }
  static int ValWrapper(TestObj* t) {
    return t->Val();
  }

  bool Alive() { return val_; }
  static bool AliveWrapper(TestObj* t) {
    return t->Alive();
  }

  void SetStr(string str) { str_ = str; }
  string Str() { return str_; }
  static string StrWrapper(TestObj* t) { return t->Str(); }

 private:
  int val_;
  string str_;
};

bool TestBooleanFilterPredicate() {
  bool success = true;
  cout << "Testing BooleanFilterPredicate" << endl;

  // Create test objects
  vector<TestObj*> test_objects;
  for (int i = 0; i < 20; ++i) {
    test_objects.push_back(new TestObj(i));
  }

  BooleanFilterPredicate<TestObj> bfp(TestObj::AliveWrapper);
  vector<TestObj*> filtered = bfp.FilterVector(test_objects);
  for (int i = 0; i < filtered.size(); ++i) {
    if (!filtered[i]->Alive()) {
      ERROR("All filtered items should have Alive() == true.");
      success = false;
    }
  }
  for (int i = 0; i < test_objects.size(); ++i) {
    delete test_objects[i];
  }
  return success;
}

bool TestGTFilterPredicate() {
  bool success = true;
  cout << "Testing GTFilterPredicate" << endl;

  // Create test objects
  vector<TestObj*> test_objects;
  for (int i = 0; i < 20; ++i) {
    test_objects.push_back(new TestObj(i));
  }

  // Create a GTFilterPredicate to keep any objects > 11.
  int val = 11;
  GTFilterPredicate<TestObj, int> gtfp(val, TestObj::ValWrapper);
  vector<TestObj*> filtered = gtfp.FilterVector(test_objects);
  for (int i = 0; i < filtered.size(); ++i) {
    cout << "Filtered[" << i << "] = " << filtered[i]->Val() << endl;
    if (filtered[i]->Val() <= val) {
      ERROR("filtered[i] wasn't > val.");
      success = false;
    }
  }

  for (int i = 0; i < test_objects.size(); ++i) {
    delete test_objects[i];
  }
  return success;
}

bool TestLTFilterPredicate() {
  bool success = true;
  cout << "Testing LTFilterPredicate" << endl;

  // Create test objects
  vector<TestObj*> test_objects;
  for (int i = 0; i < 20; ++i) {
    test_objects.push_back(new TestObj(i));
  }

  // Create a LTFilterPredicate to keep any objects < 6
  int val = 6;
  LTFilterPredicate<TestObj, int> ltfp(val, TestObj::ValWrapper);
  vector<TestObj*> filtered = ltfp.FilterVector(test_objects);
  for (int i = 0; i < filtered.size(); ++i) {
    cout << "Filtered[" << i << "] = " << filtered[i]->Val() << endl;
    if (filtered[i]->Val() >= val) {
      ERROR("filtered[i] wasn't < val.");
      success = false;
    }
  }

  for (int i = 0; i < test_objects.size(); ++i) {
    delete test_objects[i];
  }
  return success;
}

bool TestORFilterPredicate() {
  bool success = true;
  cout << "Testing OrFilterPredicate" << endl;

  // Create test objects
  vector<TestObj*> test_objects;
  for (int i = 0; i < 20; ++i) {
    test_objects.push_back(new TestObj(i));
  }

  // Create the OR filter:
  OrFilterPredicate<TestObj> or_filter;

  // Create two children:
  int val1 = 11;
  int val2 = 4;
  GTFilterPredicate<TestObj, int>* gtfp =
    new GTFilterPredicate<TestObj, int>(val1, TestObj::ValWrapper);
  LTFilterPredicate<TestObj, int>* ltfp =
    new LTFilterPredicate<TestObj, int>(val2, TestObj::ValWrapper);

  or_filter.AddChild(gtfp);
  or_filter.AddChild(ltfp);

  vector<TestObj*> filtered = or_filter.FilterVector(test_objects);
  for (int i = 0; i < filtered.size(); ++i) {
    cout << "Filtered[" << i << "] = " << filtered[i]->Val() << endl;
    if (filtered[i]->Val() <= val1 && filtered[i]->Val() >= val2) {
      ERROR("Filtered[i] wasn't > val1 or < val2.");
      success = false;
    }
  }

  for (int i = 0; i < test_objects.size(); ++i) {
    delete test_objects[i];
  }
  return success;
}

bool TestANDFilterPredicate() {
  bool success = true;
  cout << "Testing AndFilterPredicate" << endl;

  // Create test objects
  vector<TestObj*> test_objects;
  for (int i = 0; i < 20; ++i) {
    test_objects.push_back(new TestObj(i));
  }

  // Create the AND filter:
  AndFilterPredicate<TestObj> and_filter;

  // Create two children:
  int val1 = 11;
  int val2 = 4;
  GTFilterPredicate<TestObj, int>* gtfp =
    new GTFilterPredicate<TestObj, int>(val1, TestObj::ValWrapper);
  GTFilterPredicate<TestObj, int>* gtfp2 =
    new GTFilterPredicate<TestObj, int>(val2, TestObj::ValWrapper);

  and_filter.AddChild(gtfp2);
  and_filter.AddChild(gtfp);

  vector<TestObj*> filtered = and_filter.FilterVector(test_objects);
  for (int i = 0; i < filtered.size(); ++i) {
    cout << "Filtered[" << i << "] = " << filtered[i]->Val() << endl;
    if (!(filtered[i]->Val() > val1 && filtered[i]->Val() > val2)) {
      ERROR("Filtered[i] wasn't > val1 and > val2.");
      success = false;
    }
  }

  for (int i = 0; i < test_objects.size(); ++i) {
    delete test_objects[i];
  }
  return success;
}

bool TestStringContainsFilterPredicate() {
  bool success = true;
  cout << "Testing StringContainsFilterPredicate" << endl;

  // Create test objects
  vector<TestObj*> test_objects;
  for (int i = 0; i < 20; ++i) {
    test_objects.push_back(new TestObj(i));
  }

  test_objects[2]->SetStr("hi gabe");
  test_objects[3]->SetStr("gabe is cool");
  test_objects[10]->SetStr("gabe gabe gabe");
  test_objects[11]->SetStr("doesn't have the good word.");

  // Make the filter
  StringContainsFilterPredicate<TestObj> filter(string("gabe"), TestObj::StrWrapper);
  vector<TestObj*> filtered = filter.FilterVector(test_objects);
  for (int i = 0; i < filtered.size(); ++i) {
    string str = filtered[i]->Str();
    cout << "Filtered[" << i << "] = " << str << endl;
    if (str.find("gabe") == string::npos) {
      ERROR("Filtered[i] didn't contain \"gabe\"");
      success = false;
    }
  }
  assert(filtered.size() == 3);
  return success;
}

bool RunTests() {
  return TestBooleanFilterPredicate() &&
    TestGTFilterPredicate() &&
    TestLTFilterPredicate() &&
    TestORFilterPredicate() &&
    TestANDFilterPredicate() &&
    TestStringContainsFilterPredicate();
}

int main() {
  bool success = RunTests();
  cout  << errors << " errors." << endl;
  return success;
}

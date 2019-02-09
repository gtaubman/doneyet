#ifndef FILTER_PREDICATE_H_
#define FILTER_PREDICATE_H_

// This class provides a filter predicate tree to support filtering vectors
// with arbitrary function calls via the FilterVector() function.  A sample use
// case:
//
//  vector<TestObj*> test_objects;
//  BooleanFilterPredicate<TestObj> bfp(TestObj::AliveWrapper);
//  vector<TestObj*> filtered = bfp.FilterVector(test_objects);
//  for (int i = 0; i < filtered.size(); ++i) {
//    if (!filtered[i]->Alive()) {
//      ERROR("All filtered items should have Alive() == true.");
//      success = false;
//    }
//  }

#include <string>
#include <vector>

using std::string;
using std::vector;

// Filter Predicate
template <class T>
class FilterPredicate {
 public:
  FilterPredicate() { is_not_ = false; }
  virtual ~FilterPredicate() {}
  virtual bool ObjectPasses(T* t) = 0;
  virtual vector<T*> FilterVector(const vector<T*>& list) {
    vector<T*> out;
    for (int i = 0; i < list.size(); ++i) {
      if (ObjectPasses(list[i])) {
        out.push_back(list[i]);
      }
    }
    return out;
  }

  void SetIsNot(bool n) { is_not_ = n; }

 protected:
  bool is_not_;
};

template <class T>
class BooleanFilterPredicate : public FilterPredicate<T> {
 public:
  explicit BooleanFilterPredicate(bool (*bool_getter_function)(T*)) {
    bool_getter_function_ = bool_getter_function;
  }

  virtual ~BooleanFilterPredicate() {}

  bool ObjectPasses(T* t) {
    if (this->is_not_) {
      return !bool_getter_function_(t);
    }
    return bool_getter_function_(t);
  }

 private:
  bool (*bool_getter_function_)(T*);
};

// Equality Filter Predicate
template <class T1, class T2>
class EqualityFilterPredicate : public FilterPredicate<T1> {
 public:
  EqualityFilterPredicate(T2 val, T2 (*value_getter_function)(T1*)) {
    val_ = val;
    value_getter_function_ = value_getter_function;
  }
  virtual ~EqualityFilterPredicate() {}

  bool ObjectPasses(T1* t) {
    if (this->is_not_) {
      return value_getter_function_(t) != val_;
    }
    return value_getter_function_(t) == val_;
  }

 private:
  T2 val_;
  T2 (*value_getter_function_)(T1*);
};

// Greater Than Filter Predicate.  Takes two types.  The first type is the type
// in the list to be filtered.  The second type is the type of value to be
// comparing against (usually int, float, etc.).
// The following example leaves only items > 11:
//   vector<MyObj*> list;
//   GTFilterPredicate<MyObj, int> gtfp(11, MyObj::StaticValueWrapper);
//   vector<MyObj*> filtered_list = gtfp.FilterVector(list);
template <class T1, class T2>
class GTFilterPredicate : public FilterPredicate<T1> {
 public:
  GTFilterPredicate(T2 val, T2 (*value_getter_function)(T1*)) {
    val_ = val;
    value_getter_function_ = value_getter_function;
  }
  virtual ~GTFilterPredicate() {}

  bool ObjectPasses(T1* t) {
    if (this->is_not_) {
      return value_getter_function_(t) <= val_;
    }
    return value_getter_function_(t) > val_;
  }

 private:
  T2 val_;
  T2 (*value_getter_function_)(T1*);
};

// Less Than Filter Predicate
template <class T1, class T2>
class LTFilterPredicate : public FilterPredicate<T1> {
 public:
  LTFilterPredicate(T2 val, T2 (*value_getter_function)(T1*)) {
    val_ = val;
    value_getter_function_ = value_getter_function;
  }
  virtual ~LTFilterPredicate() {}

  bool ObjectPasses(T1* t) {
    if (this->is_not_) {
      return value_getter_function_(t) >= val_;
    }
    return value_getter_function_(t) < val_;
  }

 private:
  T2 val_;
  T2 (*value_getter_function_)(T1*);
};

// String Contains Filter Predicate
template <class T>
class StringContainsFilterPredicate : public FilterPredicate<T> {
 public:
  StringContainsFilterPredicate(string needle,
                                string (*text_getter_function)(T*)) {
    needle_ = needle;
    text_getter_function_ = text_getter_function;
  }
  virtual ~StringContainsFilterPredicate() {}

  virtual bool ObjectPasses(T* t) {
    const string text = text_getter_function_(t);
    if (this->is_not_) {
      return (text.find(needle_) == string::npos);
    }
    return (text.find(needle_) != string::npos);
  }

 private:
  string needle_;
  string (*text_getter_function_)(T*);
};

// AND Filter Predicate
template <class T>
class AndFilterPredicate : public FilterPredicate<T> {
 public:
  AndFilterPredicate() {}
  ~AndFilterPredicate() {
    for (int i = 0; i < children_.size(); ++i) {
      delete children_[i];
    }
  }

  virtual bool ObjectPasses(T* t) {
    for (int i = 0; i < children_.size(); ++i) {
      if (!children_[i]->ObjectPasses(t)) {
        return this->is_not_ ? true : false;
      }
    }
    if (this->is_not_) return false;
    return true;
  }

  virtual void AddChild(FilterPredicate<T>* f) { children_.push_back(f); }

  virtual void Clear() {
    for (int i = 0; i < children_.size(); ++i) {
      delete children_[i];
    }
    children_.clear();
  }

 private:
  vector<FilterPredicate<T>*> children_;
};

// OR Filter Predicate
template <class T>
class OrFilterPredicate : public FilterPredicate<T> {
 public:
  OrFilterPredicate() {}
  ~OrFilterPredicate() {
    for (int i = 0; i < children_.size(); ++i) {
      delete children_[i];
    }
  }

  virtual bool ObjectPasses(T* t) {
    for (int i = 0; i < children_.size(); ++i) {
      if (children_[i]->ObjectPasses(t)) {
        return this->is_not_ ? false : true;
      }
    }
    if (this->is_not_) return true;
    return false;
  }

  virtual void AddChild(FilterPredicate<T>* f) { children_.push_back(f); }

  virtual void Clear() {
    for (int i = 0; i < children_.size(); ++i) {
      delete children_[i];
    }
    children_.clear();
  }

 private:
  vector<FilterPredicate<T>*> children_;
};

#endif  // FILTER_PREDICATE_H_

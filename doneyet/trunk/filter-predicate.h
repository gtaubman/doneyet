#ifndef __FILTER_PREDICATE__
#define __FILTER_PREDICATE__

#include <vector>
#include <string>

// TODO:  Make inheritance actually work so I can put the "is_not_" into the
// base class.

using std::vector;
using std::string;

// Filter Predicate
template <class T> class FilterPredicate {
 public:
  FilterPredicate() {}
  virtual ~FilterPredicate() {}
  virtual bool ObjectPasses(T* t)=0;
  virtual vector<T*> FilterVector(const vector<T*>& list) {
    vector<T*> out;
    for (int i = 0; i < list.size(); ++i) {
      if (ObjectPasses(list[i])) {
        out.push_back(list[i]);
      }
    }
    return out;
  }
};

template <class T> class BooleanFilterPredicate : public FilterPredicate<T> {
 public:
  BooleanFilterPredicate(bool (*bool_getter_function)(T*)) {
    bool_getter_function_ = bool_getter_function;
    is_not_ = false;
  }

  virtual ~BooleanFilterPredicate() { }

  bool ObjectPasses(T* t) {
    return !is_not_ && bool_getter_function_(t);
  }

  void SetIsNot(bool n) { is_not_ = n; }

 private:
  bool (*bool_getter_function_)(T*);
  bool is_not_;
};

// Equality Filter Predicate
template <class T1, class T2> class EqualityFilterPredicate : public FilterPredicate<T1> {
 public:
  EqualityFilterPredicate(T2 val, T2 (*value_getter_function)(T1*)) {
    val_ = val;
    value_getter_function_ = value_getter_function;
  }
  virtual ~EqualityFilterPredicate() {}

  bool ObjectPasses(T1* t) {
    if (is_not_) {
      return value_getter_function_(t) != val_;
    }
    return value_getter_function_(t) == val_;
  }
  void SetIsNot(bool n) { is_not_ = n; }
 private:
  T2 val_;
  T2 (*value_getter_function_)(T1*);
  bool is_not_;
};

// Greater Than Filter Predicate.  Takes two types.  The first type is the type
// in the list to be filtered.  The second type is the type of value to be
// comparing against (usually int, float, etc.).
// The following example leaves only items > 11:
//   vector<MyObj*> list;
//   GTFilterPredicate<MyObj, int> gtfp(11, MyObj::StaticValueWrapper);
//   vector<MyObj*> filtered_list = gtfp.FilterVector(list);
template <class T1, class T2> class GTFilterPredicate : public FilterPredicate<T1> {
 public:
  GTFilterPredicate(T2 val, T2 (*value_getter_function)(T1*)) {
    val_ = val;
    value_getter_function_ = value_getter_function;
  }
  virtual ~GTFilterPredicate() {}

  bool ObjectPasses(T1* t) {
    return value_getter_function_(t) > val_;
  }
 private:
  T2 val_;
  T2 (*value_getter_function_)(T1*);
};

// Less Than Filter Predicate
template <class T1, class T2> class LTFilterPredicate : public FilterPredicate<T1> {
 public:
  LTFilterPredicate(T2 val, T2 (*value_getter_function)(T1*)) {
    val_ = val;
    value_getter_function_ = value_getter_function;
  }
  virtual ~LTFilterPredicate() {}

  bool ObjectPasses(T1* t) {
    return value_getter_function_(t) < val_;
  }
 private:
  T2 val_;
  T2 (*value_getter_function_)(T1*);
};

// String Contains Filter Predicate
template <class T> class StringContainsFilterPredicate : public FilterPredicate<T> {
 public:
  StringContainsFilterPredicate(string needle,
                                string (*text_getter_function)(T*)) {
    needle_ = needle;
    text_getter_function_ = text_getter_function; 
  }
  virtual ~StringContainsFilterPredicate() { }

  virtual bool ObjectPasses(T* t) {
    const string text = text_getter_function_(t);
    return (text.find(needle_) != string::npos);
  }

 private:
  string needle_;
  string (*text_getter_function_)(T*);
};

// AND Filter Predicate
template <class T> class AndFilterPredicate : public FilterPredicate<T> {
 public:
  AndFilterPredicate() {}
  ~AndFilterPredicate() {
    for (int i = 0; i < children_.size(); ++i) {
      delete children_[i];
    }
  }

  virtual bool ObjectPasses(T* t) {
    // Since it's an AND filter, we go through and if any are false all are false.
    for (int i = 0; i < children_.size(); ++i) {
      if (!children_[i]->ObjectPasses(t)) {
        return false;
      }
    }
    return true;
  }

  virtual void AddChild(FilterPredicate<T>* f) {
    children_.push_back(f);
  }

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
template <class T> class OrFilterPredicate : public FilterPredicate<T> {
 public:
  OrFilterPredicate() {}
  ~OrFilterPredicate() {
    for (int i = 0; i < children_.size(); ++i) {
      delete children_[i];
    }
  }

  virtual bool ObjectPasses(T* t) {
    // Since it's an OR filter, we go through our children and if any pass they
    // all pass.
    for (int i = 0; i < children_.size(); ++i) {
      if (children_[i]->ObjectPasses(t)) {
        return true;
      }
    }
    return false;
  }

  virtual void AddChild(FilterPredicate<T>* f) {
    children_.push_back(f);
  }

  virtual void Clear() {
    for (int i = 0; i < children_.size(); ++i) {
      delete children_[i];
    }
    children_.clear();
  }
 private:
  vector<FilterPredicate<T>*> children_;
};

#endif

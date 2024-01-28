#ifndef NOTE_H_
#define NOTE_H_

#include <string>

#include "date.h"
#include "serializer.h"

class Note {
 public:
  explicit Note(const wstring& text);
  virtual ~Note();

  wstring Text();
  wstring GetText();
  void Serialize(Serializer* s);
  void ReadFromSerializer(Serializer* s);

 private:
  Date date_;
  wstring text_;
};

#endif  // NOTE_H_

#ifndef NOTE_H_
#define NOTE_H_

#include <string>
#include "date.h"
#include "serializer.h"

class Note {
  public:
    explicit Note(const string& text);
    virtual ~Note();

    string Text();
    string GetText();
    void Serialize(Serializer* s);
    void ReadFromSerializer(Serializer* s);

  private:
    Date date_;
    string text_;
};

#endif  // NOTE_H_

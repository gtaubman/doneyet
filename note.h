#ifndef __NOTE__
#define __NOTE__

#include <string>
#include "date.h"
#include "serializer.h"

class Note {
  public:
    Note(const string& text);
    virtual ~Note();

    string Text();
    string GetText();
    void Serialize(Serializer* s);
    void ReadFromSerializer(Serializer* s);

  private:
    Date date_;
    string text_;
};

#endif

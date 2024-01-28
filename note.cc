#include "note.h"

Note::Note(const wstring& text) {
  text_ = text;
  date_.SetToNow();
}

Note::~Note() {
  // Nothing to delete.
}

wstring Note::Text() { return convToWstring(date_.ToString()) + L": " + text_; }

wstring Note::GetText() { return text_; }

void Note::Serialize(Serializer* s) {
  s->WriteString(text_);
  date_.Serialize(s);
}

void Note::ReadFromSerializer(Serializer* s) {
  text_ = s->ReadString();
  date_.ReadFromSerializer(s);
}

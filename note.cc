#include "note.h"

Note::Note(const string& text) {
    text_ = text;
    date_.SetToNow();
}

Note::~Note() {
    // Nothing to delete.
}

string Note::Text() {
    return date_.ToString() + ": " + text_;
}

string Note::GetText() {
    return text_;
}

void Note::Serialize(Serializer* s) {
    s->WriteString(text_);
    date_.Serialize(s);
}

void Note::ReadFromSerializer(Serializer* s) {
    text_ = s->ReadString();
    date_.ReadFromSerializer(s);
}

#include "serializer.h"

#include <cassert>
#include <cstring>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

Serializer::Serializer(const string& inpath, const string& outpath)
    : out_(nullptr), in_(nullptr), okay_(true) {
  if (!inpath.empty()) {
    in_ = fopen(inpath.c_str(), "rb");
    if (in_ == nullptr) {
      cout << "Error attempting to unserialize from path: " << inpath << endl;
      okay_ = false;
    }
  }

  if (!outpath.empty()) {
    out_ = fopen(outpath.c_str(), "wb");
    if (out_ == nullptr) {
      cout << "Error attempting to serialize from path: " << outpath << endl;
      okay_ = false;
    }
  }
  version_ = 0;
}

Serializer::~Serializer() {
}

void Serializer::WriteInt32(int i) { WriteUint32(static_cast<uint32>(i)); }

void Serializer::WriteUint32(uint32 ui) {
  assert(out_ != nullptr);
  size_t written = fwrite(&ui, sizeof(uint32), 1, out_);
  if (written != 1) {
      cout << "Error attempting to serialize Uint32 to file" << endl;
  }
}

void Serializer::WriteInt64(int64 i) { WriteUint64(i); }

void Serializer::WriteUint64(uint64 i) {
  WriteUint32(i >> 32);
  WriteUint32(i);
}

void Serializer::WriteString(const string& str) {
  uint32 strlength = uint32( str.length());
  WriteUint32(strlength);
  size_t written = fwrite(str.c_str(), sizeof(char), strlength, out_);
    if (written != strlength) {
        cout << "Error attempting to serialize string to file" << endl;
    }
}

uint32 Serializer::ReadUint32() {
    assert(in_ != nullptr);
    uint32 i = 0;
    size_t read = fread(&i, sizeof (uint32), 1, in_);
    if (read != 1){
        cout << "Error attempting to deserialize uint32 from file" << endl;
    }
    return i;
}

int32 Serializer::ReadInt32() { return static_cast<int32>(ReadUint32()); }

uint64 Serializer::ReadUint64() {
  uint64 i = 0;
  i |= (static_cast<uint64>(ReadUint32()) << 32);
  i |= ReadUint32();
  return i;
}

string Serializer::ReadString() {
  // First read the size of the string
  uint32 strlength = ReadUint32();
  char buffer[1000000] = {0};
  size_t read = fread(buffer, sizeof (char ), strlength, in_);
    if (read != strlength) {
        cout << "Error attempting to deserialize string from file" << endl;
    }
  return string(buffer);
}

void Serializer::CloseAll() {
  if (out_) {
      fclose(out_);
  }

  if (in_) {
      fclose(in_);
  }
}

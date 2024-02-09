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

void Serializer::CloseAll() {
    if (out_) {
        fclose(out_);
    }

    if (in_) {
        fclose(in_);
    }
}

Serializer::~Serializer() {
}

// String

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

void Serializer::WriteString(const string& str) {
  uint32 strlength = uint32( str.length());
  WriteUint32(strlength);
  size_t written = fwrite(str.c_str(), sizeof(char), strlength, out_);
    if (written != strlength) {
        cout << "Error attempting to serialize string to file" << endl;
    }
}

// UInt32
int32 Serializer::ReadInt32() { return static_cast<int32>(ReadUint32()); }

uint32 Serializer::ReadUint32() {
    assert(in_ != nullptr);
    uint32 i = 0;
    size_t read = fread(&i, sizeof (uint32), 1, in_);
    if (read != 1){
        cout << "Error attempting to deserialize uint32 from file" << endl;
    }
    return i;
}

void Serializer::WriteInt32(int i) { WriteUint32(static_cast<uint32>(i)); }

void Serializer::WriteUint32(uint32 ui) {
    assert(out_ != nullptr);
    size_t written = fwrite(&ui, sizeof(uint32), 1, out_);
    if (written != 1) {
        cout << "Error attempting to serialize Uint32 to file" << endl;
    }
}

// UInt64

uint64 Serializer::ReadUint64() {
    assert(in_ != nullptr);
    uint64 i = 0;
    size_t read = fread(&i, sizeof (uint64), 1, in_);
    if (read != 1){
        cout << "Error attempting to deserialize uint64 from file" << endl;
    }
    return i;
}

void Serializer::WriteUint64(uint64 i) {
    assert(out_ != nullptr);
    size_t written = fwrite(&i, sizeof(uint64), 1, out_);
    if (written != 1) {
        cout << "Error attempting to serialize Uint64 to file" << endl;
    }
}
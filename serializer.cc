#include "serializer.h"

#include <cassert>
#include <cstring>

#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::wstring;

Serializer::Serializer(const string& inpath, const string& outpath)
    : out_(nullptr), in_(nullptr), okay_(true) {
  if (!inpath.empty()) {
    in_ = new std::wifstream(inpath.c_str(), std::ios::in | std::ios::binary);
    if (in_->fail()) {
      cout << "Error attempting to unserialize from path: " << inpath << endl;
      delete in_;
      in_ = nullptr;
      okay_ = false;
    }
  }

  if (!outpath.empty()) {
    out_ = new std::wofstream(outpath.c_str(), std::ios::out | std::ios::binary);
    if (out_->fail()) {
      cout << "Error attempting to serialize from path: " << outpath << endl;
      delete out_;
      out_ = nullptr;
      okay_ = false;
    }
  }
  version_ = 0;
}

Serializer::~Serializer() {
  delete in_;
  delete out_;
}

void Serializer::WriteInt32(int i) { WriteUint32(static_cast<uint32>(i)); }

void Serializer::WriteUint32(uint32 ui) {
  assert(out_ != nullptr);
  const wchar_t* c = (const wchar_t*)&ui;
  out_->write(c, 1);
}

void Serializer::WriteInt64(int64 i) { WriteUint64(i); }

void Serializer::WriteUint64(uint64 i) {
  WriteUint32(i >> 32);
  WriteUint32(i);
}

void Serializer::WriteString(const wstring& str) {
  uint32 bytelength = uint32( str.length());
  WriteUint32(bytelength);
  //*out_ << str;
  out_->write(str.c_str(), bytelength);
}

uint32 Serializer::ReadUint32() {
    assert(in_ != nullptr);
    uint32 i = 0;
    wchar_t * c = (wchar_t*)&i;
    in_->read(c, 1);
    return i;
}

int32 Serializer::ReadInt32() { return static_cast<int32>(ReadUint32()); }

uint64 Serializer::ReadUint64() {
  uint64 i = 0;
  i |= (static_cast<uint64>(ReadUint32()) << 32);
  i |= ReadUint32();
  return i;
}

wstring Serializer::ReadString() {
  // First read the size of the string
  uint32 str_size = ReadUint32();

  wchar_t data[1000000] = {0};
  in_->read(data, str_size);

  return wstring(data);
}

void Serializer::Flush() {
    if (out_) {
        out_->flush();
    }

    if (in_) {
        in_->sync();
    }
}
void Serializer::CloseAll() {
  if (out_) {
    out_->close();
  }

  if (in_) {
    in_->close();
  }
}

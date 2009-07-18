#include <assert.h>
#include "serializer.h"
#include <string.h>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

static void Print(char v) {
  for (int i = 7; i >= 0; --i) {
    if (v & (1 << i)) {
      printf("1");
    } else {
      printf("0");
    }
  }
}

Serializer::Serializer(const string& inpath,
                       const string& outpath)
  : out_(NULL), in_(NULL), okay_(true), done_(false) {
  if (!inpath.empty()) {
    in_ = new std::ifstream(inpath.c_str(), std::ios::in | std::ios::binary);
    if (in_->fail()) {
      cout << "Error attempting to unserialize from path: " << inpath << endl;
      delete in_;
      in_ = NULL;
      okay_ = false;
    }
  }

  if (!outpath.empty()) {
    out_ = new std::ofstream(outpath.c_str(), std::ios::out | std::ios::binary);
    if (out_->fail()) {
      cout << "Error attempting to serialize from path: " << outpath << endl;
      delete out_;
      out_ = NULL;
      okay_ = false;
    }
  }
  version_ = 0;
}

Serializer::~Serializer() {
  delete in_;
  delete out_;
}

void Serializer::WriteInt8(int8 i) {
  WriteUint8(static_cast<uint8>(i));
}

void Serializer::WriteUint8(uint8 i) {
  assert(out_ != NULL);
  assert(!done_);
  const char* c = (const char*) &i;
  out_->write(c, 1);
}

void Serializer::WriteInt16(int16 i) {
  WriteUint16(i);
}

void Serializer::WriteUint16(uint16 i) {
  WriteUint8(i >> 8);
  WriteUint8(i);
}

void Serializer::WriteInt32(int i) {
  WriteUint32(static_cast<uint32>(i));
}

void Serializer::WriteUint32(uint32 ui) {
  WriteUint16(ui >> 16);
  WriteUint16(ui);
}

void Serializer::WriteInt64(int64 i) {
  WriteUint64(i);
}

void Serializer::WriteUint64(uint64 i) {
  WriteUint32(i >> 32);
  WriteUint32(i);
}

void Serializer::WriteFloat(float f) {
  uint32* i = (uint32*) (&f);
  WriteUint32(*i);
}

void Serializer::WriteDouble(double d) {
  uint64* i = (uint64*) (&d);
  WriteUint64(*i);
}

void Serializer::WriteString(string str) {
  int length = str.length();
  WriteUint32(length);

  for (int i = 0; i < length; ++i) {
    WriteUint8(str[i]);
  }
}

uint8 Serializer::ReadUint8() {
  assert(in_ != NULL);

  uint8 i = 0;
  char* c = (char*) &i;
  in_->read(c, 1);

  if (in_->eof()) {
    done_ = true;
  }

  return i;
}

int8 Serializer::ReadInt8() {
  return static_cast<int8>(ReadUint8());
}

uint16 Serializer::ReadUint16() {
  uint16 i = 0;
  i |= (static_cast<uint16>(ReadUint8()) << 8);
  i |= ReadUint8();
  return i;
}

int16 Serializer::ReadInt16() {
  return static_cast<int16>(ReadUint16());
}

uint32 Serializer::ReadUint32() {
  uint32 i = 0;
  i |= (static_cast<uint32>(ReadUint16()) << 16);
  i |= ReadUint16();
  return i;
}

int32 Serializer::ReadInt32() {
  return static_cast<int32>(ReadUint32());
}

uint64 Serializer::ReadUint64() {
  uint64 i = 0;
  i |= (static_cast<uint64>(ReadUint32()) << 32);
  i |= ReadUint32();
  return i;
}

int64 Serializer::ReadInt64() {
  return static_cast<int64>(ReadUint64());
}

float Serializer::ReadFloat() {
  uint32 i = ReadUint32();
  float* f = (float*) &i;
  return *f;
}

double Serializer::ReadDouble() {
  uint64 i = ReadUint64();
  double* d = (double*) &i;
  return *d;
}

string Serializer::ReadString() {
  // First read the size of the string
  uint32 str_size = ReadUint32();

  char data[1000000] = {0};
  in_->read(data, str_size);

  return string(data);
}

void Serializer::ChangeToStdBytes(char* data, long unsigned int bytes) {
  #ifndef __BIG_ENDIAN__
  // We assume that big endian is the "standard" here.  So, if we're running on
  // a big endian machine this is essentially a no-op.
  char tmp[bytes];
  for (int i = 0; i < bytes; ++i) {
    tmp[i] = data[bytes - i - 1];
  }
  memcpy(data, tmp, bytes);
  #endif
}

void Serializer::CloseAll() {
  if (out_) {
    out_->close();
  }

  if (in_) {
    in_->close();
  }
}

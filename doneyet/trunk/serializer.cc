#include "serializer.h"
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

#define TESTNOTNULL(a) \
  if (a == NULL) \
    cout << "Error, NULL stream." << endl;

#define WRITE(a) \
  { \
  TESTNOTNULL(out_) \
  char* ptr_to_data = reinterpret_cast<char*>(&(a)); \
  ChangeToStdBytes(ptr_to_data, sizeof(a)); \
  out_->write(ptr_to_data, sizeof(a)); \
  if (out_->eof()) \
    done_ = true; \
  }

#define READ(a) \
  { \
  TESTNOTNULL(in_) \
  char* ptr_to_data = reinterpret_cast<char*>(&(a)); \
  in_->read(ptr_to_data, sizeof(a)); \
  ChangeToStdBytes(ptr_to_data, sizeof(a)); \
  if (in_->eof()) \
    done_ = true; \
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
}

Serializer::~Serializer() {
  delete in_;
  delete out_;
}

void Serializer::Write(int i) {
  WRITE(i);
}

void Serializer::Write(float f) {
  WRITE(f);
}

void Serializer::Write(string str) {
  int length = str.length();
  const char* s = str.c_str();
  WRITE(length);

  for (int i = 0; i < length; ++i) {
    char c = s[i];
    WRITE(c);
  }
}

int Serializer::ReadInt() {
  int i = 0;
  READ(i);
  return i;
}

float Serializer::ReadFloat() {
  float f = 0;
  READ(f);
  return f;
}

string Serializer::ReadString() {
  // First read the size of the string
  int str_size = ReadInt();

  char data[1000] = {0};
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

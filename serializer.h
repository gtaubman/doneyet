#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <fstream>
#include <iostream>
#include <string>
#include "basic-types.h"

using std::ifstream;
using std::istream;
using std::ofstream;
using std::ostream;
using std::string;

class Serializer {
 public:
  Serializer(const string& inpath, const string& outpath);
  ~Serializer();

  void WriteUint8(uint8 i);
  void WriteUint16(uint16 i);
  void WriteInt32(int i);
  void WriteUint32(uint32 i);
  void WriteInt64(int64 i);
  void WriteUint64(uint64 i);
  void WriteString(string str);

  uint8 ReadUint8();
  uint16 ReadUint16();
  uint32 ReadUint32();
  int32 ReadInt32();
  uint64 ReadUint64();
  string ReadString();

  int Version() { return version_; }
  void SetVersion(int v) { version_ = v; }

  void CloseAll();
  bool Okay() { return okay_; }

 private:
  ofstream* out_;
  ifstream* in_;

  bool okay_;
  bool done_;

  // This is user set data to aid in passing around a file version.
  uint64 version_;
};

#endif  // SERIALIZER_H_

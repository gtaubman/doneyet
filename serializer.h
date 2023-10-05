#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <fstream>
#include <iostream>
#include <string>

#include "basic-types.h"

using std::wifstream;
using std::istream;
using std::wofstream;
using std::ostream;
using std::string;
using std::wstring;

class Serializer {
 public:
  Serializer(const string& inpath, const string& outpath);
  ~Serializer();

  void WriteInt32(int i);
  void WriteUint32(uint32 i);
  void WriteInt64(int64 i);
  void WriteUint64(uint64 i);
  void WriteString(const wstring& str);

  uint32 ReadUint32();
  int32 ReadInt32();
  uint64 ReadUint64();
  wstring ReadString();

  int Version() { return version_; }
  void SetVersion(int v) { version_ = v; }
  void Flush();

  void CloseAll();
  bool Okay() { return okay_; }

 private:
  wofstream* out_;
  wifstream* in_;

  bool okay_;

  // This is user set data to aid in passing around a file version.
  uint64 version_;
};

#endif  // SERIALIZER_H_

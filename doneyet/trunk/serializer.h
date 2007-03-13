#ifndef __SERIALIZER__
#define __SERIALIZER__

#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::ostream;
using std::ifstream;
using std::ofstream;
using std::istream;

// Unsigned
typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

// Signed
typedef long long int64;
typedef int int32;
typedef short int16;
typedef char int8;
  
class Serializer {
 public:
  Serializer(const string& inpath, const string& outpath);
  ~Serializer();

  void WriteInt8(int8 i);
  void WriteUint8(uint8 i);
  void WriteInt16(int16 i);
  void WriteUint16(uint16 i);
  void WriteInt32(int i);
  void WriteUint32(uint32 i);
  void WriteInt64(int64 i);
  void WriteUint64(uint64 i);
  void WriteFloat(float f);
  void WriteDouble(double d);
  void WriteString(string str);

  uint8 ReadUint8();
  int8 ReadInt8();
  uint16 ReadUint16();
  int16 ReadInt16();
  uint32 ReadUint32();
  int32 ReadInt32();
  int64 ReadInt64();
  uint64 ReadUint64();
  float ReadFloat();
  double ReadDouble();
  string ReadString();

  void CloseAll();
  bool Okay() { return okay_; }
  bool Done() { return done_; }

 private:
  inline void ChangeToStdBytes(char* data, long unsigned int bytes);
  
  ofstream* out_;
  ifstream* in_;

  bool okay_;
  bool done_;
};

#endif

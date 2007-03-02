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

class Serializer {
 public:
  Serializer(const string& inpath, const string& outpath);
  ~Serializer();

  void Write(int i);
  void Write(float f);
  void Write(string str);

  int ReadInt();
  float ReadFloat();
  string ReadString();

  void CloseAll();
  bool Okay() { return okay_; }
  bool Done() { return done_; }

 private:
  inline char EndianIndependentByte(unsigned char byte);
  inline void ChangeToStdBytes(char* data, long unsigned int bytes);
  
  ofstream* out_;
  ifstream* in_;

  bool okay_;
  bool done_;
};

#endif

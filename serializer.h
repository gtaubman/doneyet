#ifndef __SERIALIZER__
#define __SERIALIZER__

#include <string>
#include <iostream>
#include <fstream>
#include "basic-types.h"

using std::string;
using std::ostream;
using std::ifstream;
using std::ofstream;
using std::istream;

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

    int Version() {
        return version_;
    }
    void SetVersion(int v) {
        version_ = v;
    }

    void CloseAll();
    bool Okay() {
        return okay_;
    }
    bool Done() {
        return done_;
    }

  private:
    inline void ChangeToStdBytes(char* data, long unsigned int bytes);

    ofstream* out_;
    ifstream* in_;

    bool okay_;
    bool done_;

    // This is user set data to aid in passing around a file version.
    uint64 version_;
};

#endif

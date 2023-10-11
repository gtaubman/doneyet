#include <locale.h>
#include <string>
#include "serializer.h"
using std::wstring;

int main() {
    setlocale(LC_ALL, "");
    int mynum = 42;
    wstring mystring =  L"input öäü fiancé: ÂÄ many more words";
    Serializer* s = new Serializer("", "./bla");
    s->WriteUint32(mynum);
    s->WriteString(mystring);
    s->CloseAll();
    delete s;
    s = new Serializer("./bla", "./bla2");
    int compnum = s->ReadUint32();
    if (compnum != mynum) {
        printf("Serializer Test: the numbers do not match\n");
    }
    wstring compstring;
    compstring= s->ReadString();
    if (compstring != mystring) {
        printf("Serializer Test: the strings do not match\n");
    }
    s->CloseAll();
    printf("See me if all tests pass\n");
    delete s;
    return 0;
}

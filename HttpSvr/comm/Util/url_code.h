
#ifndef ___URL_CODE___HHH___
#define ___URL_CODE___HHH___

#include <string>
using namespace std;

//URL解码
string urldecode(string &str_source);
//URL编码
string urlencode(string &str_source);
string urlencode(const char* str_source, int nLen);

#endif
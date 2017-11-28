//
// Created by new on 2016/11/9.
//

#ifndef HTTPSVR_LIB_IP_H
#define HTTPSVR_LIB_IP_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class Lib_Ip {
public:
    static FILE* fp;
    static map<string, vector<string> > cached;
    static char *index;
    static int offset;

    static bool init();

    static vector<string> find(const char* nip);
};


#endif //HTTPSVR_LIB_IP_H

//
// Created by new on 2016/11/9.
//

#include "Lib_Ip.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "threadres.h"
#include "Util.h"
#include "Helper.h"
#include "StrFunc.h"

FILE* Lib_Ip::fp = NULL;
char* Lib_Ip::index = NULL;
int Lib_Ip::offset = 0;
map<string, vector<string> > Lib_Ip::cached;

int unpack(const char *format, char data1, char data2, char data3, char data4) {
    int val = 0;
    char *p = (char*)&val;
    p[0] = data1;
    p[1] = data2;
    p[2] = data3;
    p[3] = data4;
    if (!strcmp(format,"N"))
        return htonl(val);

    return val;
}

int unpack(const char *format, char *data) {
    int val = *(int*)data;
    if (!strcmp(format,"N"))
        return htonl(val);

    return val;
}

bool Lib_Ip::init() 
{
    if (!fp) 
	{
        string filepath = Util::getModulePath() + "/../conf/17monipdb.dat";
        fp = fopen(filepath.c_str(), "rb");
        if (!fp)
            return false;

        char buf[4];
        int nread = fread(buf, 1, 4, fp);
        if (nread < 4)
            return false;
        offset = unpack("N", buf);
        index = (char*)malloc(offset - 4);
        nread = fread(index, 1, offset - 4, fp);
        if (nread < offset - 4)
            return false;
    }

    return true;
}

vector<string> Lib_Ip::find(const char* nip)
{
    vector<string> ret;
    if (NULL == nip || 0 == strlen(nip))
    {
        return ret;
    }

    vector<string> ipdot_str = Helper::explode(nip, ".");
    std::vector<int> ipdot;
    for (vector<string>::iterator it = ipdot_str.begin(); it != ipdot_str.end(); it++)
        ipdot.push_back(atoi(it->c_str()));

    if (ipdot[0] < 0 || ipdot[0] > 255 || ipdot.size() != 4)
    {
        return ret;
    }
	RedisAccess* rdCommon = ThreadResource::getInstance().getRedisConnMgr()->common();
	if(NULL == rdCommon)
	{
		return ret;
	}
	std::string strKey = StrFormatA("CPP|ip_%s",nip);
	std::string strRet;
	if(rdCommon->GET(strKey.c_str(),strRet))
	{
		ret = Helper::explode(strRet.c_str(), "\t");
		return ret;
	}

    if (cached.find(nip) != cached.end())
    {
        return cached[nip];
    }

	init();
	if (NULL == index)
	{
		LOGGER(E_LOG_ERROR) << "index is NULL(17monipdb.dat not found)";
		return ret;
	}

    //big-endian
	//std::string strNIP2 = StrFormatA("PAK%d",Helper::ip2long(nip));
    int nip2 = Helper::ip2long(nip);

    int tmp_offset = (int)ipdot[0] * 4;
    int start = unpack("V", index[tmp_offset], index[tmp_offset+1], index[tmp_offset+2], index[tmp_offset+3]);
    int index_offset = 0;
    int index_length = 0;
    int max_comp_len = offset - 1024 - 4;
    for (start = start * 8 + 1024; start < max_comp_len; start += 8)
    {
        if (unpack("N", index+start) >= nip2)
        {
            index_offset = unpack("V", index[start + 4], index[start + 5], index[start + 6], 0);
            index_length = *(unsigned char*)(index + start + 7);
            break;
        }
    }

    if (index_offset == 0)
    {
        return ret;
    }

    fseek(fp, offset + index_offset - 1024, SEEK_SET);

    char *buf = (char*)malloc(index_length+1);
    fread(buf, 1, index_length, fp);
    buf[index_length] = 0;
    cached[nip] = Helper::explode(buf, "\t");
    //缓存2天
	rdCommon->SET(strKey.c_str(), buf, 2*24*3600);

    free(buf);


    return cached[nip];
}

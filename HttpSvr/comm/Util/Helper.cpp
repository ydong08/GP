//
// Created by new on 2016/11/8.
//
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <arpa/inet.h>
#include "Helper.h"
#include "StrFunc.h"

#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <net/if.h> 
#include <netinet/if_ether.h>

string Helper::filterInput(string str)
{
	if (str.empty()) return "";

    // 需要替换的特殊字符
    string ret;
    char specialStr[41] = {'\\', '\'', '"', '`', '&', '/', '<', '>', '\x7f'};
    for (int i = 0; i < 32; i++ )
        specialStr[9+i] = (char)i;

    for (int i = 0; i < str.length(); i++) {
        char c = str.at(i);
        int j;
        for (j=0;j<sizeof(specialStr);j++)
            if (specialStr[j] == c)
                break;
        if (j == sizeof(specialStr)) {
            ret.push_back(c);
        }
    }

    return ret;
}

int Helper::strtotime(string date) 
{
    struct tm t;
    unsigned long time;

    sscanf(date.c_str(),"%d-%d-%d %d:%d:%d",&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec);

    t.tm_year-=1900;
    t.tm_mon-=1;
    time=mktime(&t);      //转换
    return time;
}

vector<string> Helper::explode(const char * str, const char * sep)
{
    char *srcstr = strdup(str);
    vector<string> result;
    char *saveptr = NULL;
    char *p = strtok_r(srcstr, sep, &saveptr);
    while (p) {
        result.push_back(p);
        p = strtok_r(NULL, sep, &saveptr);
    }
    free(srcstr);
    return result;
}

string Helper::implode(const char *sep, const vector<string>& strarr) 
{
    string strRet = "";
    for (int i = 0; i < strarr.size(); i++) 
	{
		if (!strRet.empty()) strRet += sep;
        strRet += strarr[i];
    }
    return strRet;
}

std::string Helper::implode( const char* sep, const Json::Value& strarr )
{
	std::string strRet = "";
	if (strarr.type() == Json::ValueType::objectValue)
	{
		Json::Value::Members mem = strarr.getMemberNames();
		for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++) 
		{
			if (!strRet.empty()) strRet += sep;
			strRet += strarr[*iter].asString();
		}
	}
	else if(strarr.type() == Json::ValueType::arrayValue)
	{
		int nMaxNum = strarr.size();
		for (int i = 0 ; i < nMaxNum; i++)
		{
			strRet += strarr[i].asString();
		}
	}
	return strRet;
}

std::string Helper::implode(const char *sep, const std::vector<int>& vec)
{
    std::string str = "";
    for (size_t i = 0; i < vec.size(); i++)
    {
        str = StrFormatA("%s%d%s", str.c_str(), vec[i], sep);
    }
    //gcc 4.4不支持string::pop_back
    str.erase(str.end()-1); //去除尾部的split
    return str;
}

string Helper::strtolower(const char* str)
{
    string ret = str;
    ToLower(ret);
    return  ret;
}

string Helper::trim(const char *str) {
    return TrimStr(str);
}

const char *charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
bool Helper::isUsername(const char *str) {
    int len = strlen(str);
    if (len < 3 || len > 16)
        return false;

    while(*str && !strchr(charset, *str))
        return false;

    return true;
}

long Helper::ip2long(const char *ip) {
    return ntohl(inet_addr(ip));
}

std::string Helper::long2ip(long ip)
{
    in_addr addr;
    addr.s_addr = ip;
    char* saddr = inet_ntoa(addr);
    if (saddr != NULL)
    {
        return std::string(saddr);
    }
    return "";
}

string Helper::replace(const string& str, const string& src, const string& dest)
{
    string ret;

    string::size_type pos_begin = 0;
    string::size_type pos       = str.find(src);
    while (pos != string::npos)
    {
        ret.append(str.data() + pos_begin, pos - pos_begin);
        ret += dest;
        pos_begin = pos + 1;
        pos = str.find(src, pos_begin);
    }
    if (pos_begin < str.length())
    {
        ret.append(str.begin() + pos_begin, str.end());
    }
    return ret;
}

int Helper::time2morning() {
    time_t now = time(NULL);
    struct tm today = *localtime(&now);
    double seconds;
    today.tm_hour = 0; today.tm_min = 0; today.tm_sec = 0;

    seconds = 24*60*60-difftime(now,mktime(&today));
    return (int)seconds;
}

std::string Helper::GetPeerMac( int sockFd )
{
	struct arpreq arpReq;
	struct sockaddr_storage dstadd_in;	//注意IPV6
	socklen_t len;
	memset(&arpReq,0,sizeof(struct arpreq));
	memset(&dstadd_in,0,sizeof(struct sockaddr_storage));
	if (getpeername(sockFd,(struct sockaddr*)&dstadd_in,&len) < 0)  return "";

	memcpy(&arpReq.arp_pa,&dstadd_in,sizeof(struct sockaddr_in));
	strcpy(arpReq.arp_dev,"eth1");
	arpReq.arp_pa.sa_family = dstadd_in.ss_family;		//兼容IPV6

	if(ioctl(sockFd, SIOCGARP, &arpReq) < 0) return "";

	unsigned char * hw_addr = (unsigned char *) arpReq.arp_ha.sa_data;
	return StrFormatA("%02x%02x%02x%02x%02x%02x", *hw_addr, *(hw_addr+1), *(hw_addr+2), *(hw_addr+3), *(hw_addr+4), *(hw_addr+5));
}

void unsetUserFiled(Json::Value& jvUser) 
{
	std::string strUnsetFiled[] = {"bankPWD", "device_no", "ipArr", "mtype", "deviceToken", "aMactivetime", "aMentercount", "aMtime", "aPid", "aVersions", "devicename", "osversion", "nettype", 
		"vendor", "gameid", "cid", "pid", "ctype", "versions", "mtime", "mstatus", ""};
	int i = 0;
	while (!strUnsetFiled[i].empty())
	{
		if (!jvUser[strUnsetFiled[i]].isNull()) jvUser.removeMember(strUnsetFiled[i]);
		i++;
	}
}
int32_t Helper::getCurMonthTimeStamp() 
{
	time_t t = time(0);   // get time now
	struct tm * zeroTm= localtime(&t);
	if(zeroTm != NULL)
	{
		zeroTm->tm_hour = 0;
		zeroTm->tm_min = 0;
		zeroTm->tm_sec = 0;
		zeroTm->tm_mday = 1;
		unsigned long long zeroTime = mktime(zeroTm);
		return zeroTime;
	}
	else
	{
		return 0;
	}
}

int32_t Helper::getNextMonthTimeStamp() 
{
	time_t t = time(0);   // get time now
	struct tm * zeroTm= localtime(&t);
	if(zeroTm != NULL)
	{
		zeroTm->tm_hour = 0;
		zeroTm->tm_min = 0;
		zeroTm->tm_sec = 0;
		zeroTm->tm_mday = 1;
		if(zeroTm->tm_mon == 11)
		{
			zeroTm->tm_mon = 0;
		}
		else
		{
			zeroTm->tm_mon = zeroTm->tm_mon + 1;
		}
		unsigned long long zeroTime = mktime(zeroTm);
		return zeroTime;
	}
	else
	{
		return 0;
	}
}


std::string Helper::getTimeStampToYearMonthTime(int timestamp) 
{
	time_t t = timestamp;   // get time now
	struct tm * zeroTm= localtime(&t);
	if(zeroTm != NULL)
	{
		//mon 是 0~11 所以+1
		return StrFormatA("%d.%d",zeroTm->tm_year+1900,zeroTm->tm_mon+1);
	}
	else
	{
		return StrFormatA("");
	}
}


std::string Helper::getCurYearMonthTime() 
{
	time_t t = time(0);   // get time now
	struct tm * zeroTm= localtime(&t);
	if(zeroTm != NULL)
	{
		return StrFormatA("%d.%d",zeroTm->tm_year+1900,zeroTm->tm_mon);
	}
	else
	{
		return StrFormatA("");
	}
}
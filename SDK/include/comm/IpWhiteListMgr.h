#ifndef _IP_WHITE_LIST_MGR_H_
#define _IP_WHITE_LIST_MGR_H_

#include<time.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <string.h>
#include <stdio.h>
#include "Util.h"


#include "CDL_Timer_Handler.h"

class IPWhiteListMgr : public CCTimer
{
public:
    static IPWhiteListMgr* getInstance()
    {
        static IPWhiteListMgr* ins = new IPWhiteListMgr();
        return ins;
    }

    IPWhiteListMgr() { }

    void Init(std::string file)
    {
        m_sFileName = file;
        m_time = _get_ip_white_list_modify_time(m_sFileName);
    }

    virtual ~IPWhiteListMgr(){ m_IpWhiteList.clear(); }

    int Load();

    bool IsInIPWhiteList(std::string &ip);

private:
    time_t _get_ip_white_list_modify_time(std::string file)
    {
        FILE * fp;
        int fd;
        struct stat status;
        fp = fopen(file.c_str(), "r");
        fd = fileno(fp);
        fstat(fd, &status);
        fclose(fp);
        return status.st_mtime;
    }

    bool _is_conf_changed()
    {
        return m_time != _get_ip_white_list_modify_time(m_sFileName) ? true : false;
    }

    virtual int ProcessOnTimerOut()
    {
        if (_is_conf_changed())
        {
            Load();
        }

        return 0;
    }


private:
    std::string           m_sFileName;
    time_t                m_time;
    std::map<std::string, std::string> m_IpWhiteList;  //<ip, desc>
};


#endif //Offline_Redis_H


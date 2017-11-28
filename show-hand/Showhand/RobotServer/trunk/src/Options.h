
#ifndef OPTIONS_H
#define OPTIONS_H

#include <time.h>
#include <unistd.h>
#include "Typedef.h"
#include <string>
 
class Options
{
public:

	static Options* instance() {
		static Options * options = NULL;
		if(options==NULL)
			options = new Options();
		return options;
	}

	int parse_args(int argc, char *argv[]);

	int read_conf(const char file[256]);

	void printConf();

//***********************************************************
public:
	//***************监听Server**************
	//游戏监听Admin
	char listen_address[256];
	short port;

	//连接大厅的ip和端口
	char hall_ip[64];
	short hall_port;

	char redis_ip[64];
	short redis_port;

	//日志
	int			  m_loglevel;
	std::string   m_logRemoteIp;
	UINT16        m_logRemotePort;

	time_t lasttime;
	//扫描监控时间
	short monitortime;
	//默认下注超时时间，可以再其上在加
	short basebettime;
	//连续和用户玩的牌局数
	short baseplayCount;
	//用户等待时间就开启机器人
	short entertime;

	UINT16 m_robotId;	//第几个机器人服务器

	short level;
	//机器人明牌打等级
	short konwcontrlLevel;

	int swtichWin1;
	int swtichWin2;
	int swtichWin3;
	
	int baseMoney1;
	int baseMoney2;
	int baseMoney3;
	int randMoney1;
	int randMoney2;

	int baseuid;
	int randuid;

	//验证服务器ip和端口
    char verifyredis_ip1[64];
    short verifyredis_port1;
	//验证服务器ip和端口
    char verifyredis_ip2[64];
    short verifyredis_port2;

	short getinredis;

	//头像链接
	std::string   m_headLink;
private:
	Options();
};

#define _CFG_(key)  Options::instance()->key  
#endif


#ifndef _Configure_H
#define _Configure_H 

#include <time.h>
#include <unistd.h>
#include <string>
#include "Typedef.h"

typedef struct _checkHost
{
	char host[32];
	short port;
}checkHost;

#define MAX_ALLOC_NUM  8
class Configure
{
public:

	static Configure* getInstance() {
		static Configure * configure = NULL;
		if(configure==NULL)
			configure = new Configure();
		return configure;
	}

	int parse_args(int argc, char *argv[]);

	int read_conf(const char file[256]);

	void printConf();

//***********************************************************
public:
	short server_id; //服务器ID
	short level;//服务器等级 
	//监听Server
	char listen_address[64];
	short port;	// 服务器端口;
	int server_priority;	// 优先权

	char alloc_ip[64];
	short alloc_port;
	short numplayer;

	char mysql_ip[64];
	short mysql_port;

	//日志上报
	char log_server_ip[64];
	int log_server_port;
	int isLogReport;

	int			  m_loglevel;
	std::string   m_logRemoteIp;
	UINT16        m_logRemotePort;
	
	char round_ip[64];
	short round_port;
	
	//控制进入桌子
	short contrllevel;
 
	//第一轮最大倍数
	short maxmulone;
	//第二轮最大倍数
	short maxmultwo;
	//上报IP
	char report_ip[64]; 

	//日志
	char logfile[64];
	char loglevel[12];

    int monitor_time;//监控时间
    int keeplive_time;//存活时间

	int max_table;
	int max_user;
	//下注的超时时间
	int betcointime;
	//大于两个人准备其中有人没有准备，然后倒计时把没准备的人踢出，并且游戏开始
	int tablestarttime;
	//超时踢出没准备用户
	short kicktime;

	short loserate;

	time_t starttime; //服务器启动时间
	time_t lasttime;  //最近活动时间
	//下低注就超时几次之后就把他踢出
	short timeoutCount;

	short fraction;

	//屏蔽词服务器
	char word_server_ip[64];
	short word_server_port;

	//udp服务器ip和端口
    char udp_ip[64];
    short udp_port;

	//redis服务器ip和端口 获取任务的配置信息
    char redis_ip[64];
    short redis_port;
	//redis服务器ip和端口 储存用户的任务历史信息
    char redis_tip[64];
    short redis_tport;
	//redis服务器ip和端口 保存用户的新手任务信息
    char redis_nip[64];
    short redis_nport;

	//redis服务器ip和端口 局数任务用到的redis端口
    char taskredis_ip[64];
    short taskredis_port;

    //掉线玩家数据
    char offline_redis_ip[64];
    short offline_redis_port;

	//简单任务的发放局数条件
	short esayTaskCount;
	short esayRandNum;
	short esayTaskRand;

	//当天前n局
	short curBeforeCount;
	//当天前N局游戏获取简单任务的概率
	short esayTaskProbability;

	//获得乐劵通知的个数条件
	short getIngotNoti1;
	short getIngotNoti2;
	short getIngotNoti3;

	short robotTabNum1;
	short robotTabNum2;
	short robotTabNum3;

	int rewardcoin;
	int rewardroll;
	short rewardRate;

	//赢取金币小喇叭发送
	int wincoin1;
	int wincoin2;
	int wincoin3;
	int wincoin4;

	checkHost CheckInGame[10];

	//redis服务器ip和端口 运维数据用到的redis端口
	char operation_redis_ip[64];
	short operation_redis_port;
private:
	Configure();
};

#endif

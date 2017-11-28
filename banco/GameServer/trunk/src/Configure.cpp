#include <sys/stat.h> 
#include <signal.h> 
#include "Configure.h"
#include "Logger.h"
#include "Util.h"
#include "Protocol.h"

#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#endif

Configure::Configure() : GameConfig(E_BACCARAT_GAME_ID, E_MSG_SOURCE_SERVER)
{
	max_table = 0;
	max_user = 0;

	starttime = time(NULL);
	lasttime = starttime;
}

bool Configure::LoadGameConfig()
{
	//最大棋桌 最大人数
	if (max_table == 0)
		max_table = ReadInt("Room", "MaxTable", 500);
	if (max_user == 0)
		max_user = ReadInt("Room", "MaxUser", 5);

	monitor_time = ReadInt("Room", "MonitorTime", 30);
	keeplive_time = ReadInt("Room", "KeepliveTime", 100);
	ready_time = ReadInt("Room", "ReadyTime", 6);
	idletime = ReadInt("Room", "IdleTime", 5);
	opentime = ReadInt("Room", "OpenTime", 10);
	bettime = ReadInt("Room", "BetTime", 15);
	reset_time = ReadInt("Room", "ResetTime", 2);
	rentrate = ReadInt("Room", "RentRate", 2);
	switchchat = ReadInt("Room", "SwitchChat", 0);
	bankerwintype = ReadInt("Room", "WinType", 8);
	bankerlosetype = ReadInt("Room", "LoseType", 3);
	areawintype = ReadInt("Room", "AreaWinType", 8);
	switchbetmoney = ReadInt("Room", "SwitchLoseMoney", 2000000);
	randlose = ReadInt("Room", "RandLose", 70);
	kicktime = ReadInt("Room", "KickTime", 300);
	switchsendsize = ReadInt("Room", "SwitchSize", 7);
    robotBankerWinRate = ReadInt("Room", "RobotBankerWinRate", 2);

	wincoin1 = ReadInt("Room", "TrumptCoin1", 50000);
	wincoin2 = ReadInt("Room", "TrumptCoin2", 10000);
	wincoin3 = ReadInt("Room", "TrumptCoin3", 5000);
	wincoin4 = ReadInt("Room", "TrumptCoin4", 2000);

	char key[15];
	sprintf(key, "Alloc_%d", this->m_nLevel);
	//匹配服务器
	std::string alloc_host = ReadString(key, "Host", "NULL");
	strcpy(alloc_ip, alloc_host.c_str());
	alloc_port = ReadInt(key, "Port", 0);
	if (alloc_port == 0) {
		return false;
	}

	numplayer = ReadInt(key, "NumPlayer", 5);

	switchrobotrank = ReadInt("Switch", "RobotRank", 0);

	return true;
}

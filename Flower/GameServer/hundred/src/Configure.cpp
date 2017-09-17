#include <sys/stat.h>
#include <signal.h>
#include "Configure.h"
#include "Logger.h"
#include "Protocol.h"


#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#endif

Configure::Configure() : GameConfig(GAME_TYPE, E_MSG_SOURCE_SERVER, E_CONFIG_GAME)
{
	max_table = 0;
	max_user = 0;

	starttime = time(NULL);
	lasttime = starttime;
}

Configure::~Configure()
{
}

bool Configure::LoadGameConfig()
{
	//最大棋桌 最大人数
	max_table = ReadInt("Room", "MaxTable", 1);
	max_user = ReadInt("Room", "MaxUser", 512);
	monitor_time = ReadInt("Room" ,"MonitorTime", 30);
	keeplive_time  = ReadInt("Room" ,"KeepliveTime", 100);
	idletime = ReadInt("Room" ,"IdleTime", 5);
	opentime = ReadInt("Room" ,"OpenTime", 15);
	bettime =  ReadInt("Room" ,"BetTime", 15);
	wincoin1 = ReadInt("Room", "TrumptCoin1", 50000);
	wincoin2 = ReadInt("Room", "TrumptCoin2", 10000);
	wincoin3 = ReadInt("Room", "TrumptCoin3", 5000);
	wincoin4 = ReadInt("Room", "TrumptCoin4", 2000);
	switchsendsize = ReadInt("Room" ,"SwitchSize", 7);
	rentrate = ReadInt("Room" ,"RentRate", 2);
	switchchat = ReadInt("Room" ,"SwitchChat", 0);
	bankerwintype = ReadInt("Room" ,"WinType", 8);
	bankerlosetype = ReadInt("Room" ,"LoseType", 3);
	areawintype = ReadInt("Room" ,"AreaWinType", 8);
	switchbetmoney = ReadInt("Room" ,"SwitchLoseMoney", 2000000);
	randlose = ReadInt("Room" ,"RandLose", 70);
	jackpotSwitch = ReadInt("Room" ,"SwitchJackPot", 50000000);
	randopenjack = ReadInt("Room" ,"RandOpenJack", 50);
	robotbetrate = ReadInt("Room" ,"RobotBetRate", 90);
	specialnum0 = ReadInt("Room", "SNum0", 100);
	specialnum1 = ReadInt("Room", "SNum1", 100);
	specialnum2 = ReadInt("Room", "SNum2", 100);
	specialnum3 = ReadInt("Room", "SNum3", 100);
	kicktime = ReadInt("Room" ,"KickTime", 300);
	sendinfotime = ReadInt("Room" ,"SendInfoTime", 1000);

	char key[15];
	sprintf(key,"Alloc_%d",this->m_nLevel);
	//匹配服务器
	std::string alloc_host =  ReadString(key,"Host","NULL");
	strcpy( alloc_ip, alloc_host.c_str() );
    alloc_port = ReadInt(key,"Port",0);
	numplayer = ReadInt(key,"NumPlayer",4);
	robotwin = ReadInt(key,"RobotWin",20000);

	bankerlimit = ReadInt("Room" ,"Bankerlimit", 100000);
	bankernum = ReadInt("Room" ,"BankerNum", 5);

	switchrobotrank = ReadInt("Switch","RobotRank",0);
	return true;

}


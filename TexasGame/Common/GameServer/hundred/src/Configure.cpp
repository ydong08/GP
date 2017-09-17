
#include "Configure.h"
#include "Logger.h"
#include "Protocol.h"
#include "Util.h"

Configure::Configure() : GameConfig(E_SHOWHAND_GAME_ID, E_MSG_SOURCE_SERVER)
{
	max_table = 0;
	max_user = 0;

	starttime = time(NULL);
	lasttime = starttime;
}

bool Configure::LoadGameConfig()
{ 
	//最大棋桌 最大人数
	max_table = ReadInt("Room" ,"MaxTable", 1);
	max_user  = ReadInt("Room" ,"MaxUser", 512);

	monitor_time = ReadInt("Room" ,"MonitorTime", 30);
	keeplive_time  = ReadInt("Room" ,"KeepliveTime", 100);
	idletime = ReadInt("Room" ,"IdleTime", 5);
	opentime = ReadInt("Room" ,"OpenTime", 15);
	bettime = ReadInt("Room" ,"BetTime", 15);
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
	//jackpotSwitch = iniFile.ReadInt("Room" ,"SwitchJackPot", 50000000);
	randopenjack = ReadInt("Room" ,"RandOpenJack", 50);
	robotbetrate = ReadInt("Room" ,"RobotBetRate", 90);
	specialnum0 = ReadInt("Room","SNum0",100);
	specialnum1 = ReadInt("Room","SNum1",100);
	specialnum2 = ReadInt("Room","SNum2",100);
	specialnum3 = ReadInt("Room","SNum3",100);
	specialnum4 = ReadInt("Room","SNum4",100);
	specialnum5 = ReadInt("Room","SNum5",100);
	specialnum6 = ReadInt("Room","SNum6",100);
	kicktime = ReadInt("Room" ,"KickTime", 300);
	sendinfotime = ReadInt("Room" ,"SendInfoTime", 1000);

	char key[15];
	sprintf(key,"Alloc_%d",this->m_nLevel);	
	//匹配服务器
	alloc_host =  ReadString(key,"Host","null");
    alloc_port = ReadInt(key,"Port",0);
	numplayer = ReadInt(key,"NumPlayer",4);
	robotwin = ReadInt(key,"RobotWin",20000);

	bankerlimit = ReadInt("Room" ,"Bankerlimit", 5000000);
	bankernum = ReadInt("Room" ,"BankerNum", 5);

	switchrobotrank = ReadInt("Switch","RobotRank",0);

	return true;
}


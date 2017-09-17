#include <sys/stat.h>
#include <signal.h>
#include "IniFile.h"
#include "Configure.h"
#include "Logger.h"
#include "Protocol.h"

#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#endif

Configure::Configure() : GameConfig(GAME_TYPE, E_MSG_SOURCE_SERVER)
{
	starttime = time(NULL);
	lasttime = starttime;
}

bool Configure::LoadGameConfig()
{
	//匹配服务器
	char		key[15];

	sprintf(key, "Alloc_%d", m_nLevel);
	std::string alloc_host = ReadString(key, "Host", "NULL");
	strcpy(alloc_ip, alloc_host.c_str());
	alloc_port = ReadInt(key, "Port", 0);
	if (0 == alloc_port)
	{
		printf("allocinfo alloc_port is null is error or mysql_port or log_server is null or redis_port is null level:%d\n", m_nLevel);
		return false;
	}
	max_table = ReadInt("Room", "MaxTable", 100);
	max_user = ReadInt("Room", "MaxUser", 500);
	monitor_time = ReadInt("Room" ,"MonitorTime", 30);
	keeplive_time  = ReadInt("Room" ,"KeepliveTime", 100);
	wincoin1 = ReadInt("Room", "TrumptCoin1", 50000);
	wincoin2 = ReadInt("Room", "TrumptCoin2", 10000);
	wincoin3 = ReadInt("Room", "TrumptCoin3", 5000);
	wincoin4 = ReadInt("Room", "TrumptCoin4", 2000);

	max_round = ReadInt("Room" ,"MaxRound", 20);
	//compare_round = ReadInt("Room" ,"CompareRound", 2);
	allin_round = ReadInt("Room" ,"AllinRound", 6);

	//check_round = ReadInt("Room" ,"CheckRound", 1);
	contrllevel = ReadInt("Room" ,"ContrlLevel", 2);

	numplayer = ReadInt(key,"NumPlayer",4);
	rewardcoin = ReadInt(key,"RewardCoin",1000000);
	rewardroll = ReadInt(key,"RewardRoll",1000000);
	rewardRate = ReadInt(key,"RewardRate",80);

	mulcount = ReadInt(key,"MulCount",1);
	mulnum1 = ReadInt(key,"MulNum1",1);
	mulcoin1 = ReadInt(key,"MulCoin1",1);
	mulnum2 = ReadInt(key,"MulNum2",1);
	mulcoin2 = ReadInt(key,"MulCoin2",1);

	forbidcount = ReadInt(key,"ForbidCount",1);
	forbidnum1 = ReadInt(key,"ForbidNum1",1);
	forbidcoin1 = ReadInt(key,"ForbidCoin1",1);
	forbidnum2 = ReadInt(key,"ForbidNum2",1);
	forbidcoin2 = ReadInt(key,"ForbidCoin2",1);
	betmaxallinflag = ReadInt(key ,"BetMaxAllinFlag", 1);

	changecount = ReadInt(key,"ChangeCount",1);
	changecoin1 = ReadInt(key,"ChangeCoin1",1);
	changecoin2 = ReadInt(key,"ChangeCoin2",1);
	changecoin3 = ReadInt(key,"ChangeCoin3",1);

	maxmulone = ReadInt("MaxMul","MulOneRound",4);
	maxmultwo = ReadInt("MaxMul","TwoOneRound",5);

	betcointime = ReadInt("Timer","BetCoinTime",20);
	tablestarttime = ReadInt("Timer", "TableStartTime", 10);
	kicktime = ReadInt("Timer","KickTime",10);

	timeoutCount = ReadInt("TimeOutCount","num",2);
	fraction = ReadInt("Fraction","num",100);

	esayTaskCount = ReadInt("EsayTask","PlayCount",30);
	esayRandNum = ReadInt("EsayTask","RandNum",60);
	esayTaskRand = ReadInt("EsayTask","TaskRand",50);

	getIngotNoti1 = ReadInt("Notify","Num1",10);
	getIngotNoti2 = ReadInt("Notify","Num2",10);
	getIngotNoti3 = ReadInt("Notify","Num3",10);

	robotTabNum1 = ReadInt("RobotTab","Num1",10);
	robotTabNum2 = ReadInt("RobotTab","Num2",10);
	return true;
}

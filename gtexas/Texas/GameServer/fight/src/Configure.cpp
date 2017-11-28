#include "Configure.h"
#include "Protocol.h"

Configure::Configure() : GameConfig(E_TEXAS_GAME_ID, E_MSG_SOURCE_SERVER, E_CONFIG_GAME)
{
	level = 0;
	server_id = 0;
	server_priority = 0 ;
	max_table = 0;
	max_user = 0;

	starttime = time(NULL);
	lasttime = starttime;
}

bool Configure::LoadGameConfig() 
{
	level = m_nLevel;
	server_id = m_nServerId;
	server_priority = m_nWeight;
	//最大棋桌 最大人数
	if(max_table==0)
		max_table = ReadInt("Room" ,"MaxTable", 100);
	if(max_user==0)
		max_user  = ReadInt("Room" ,"MaxUser", 500);

	monitor_time = ReadInt("Room" ,"MonitorTime", 30);
	keeplive_time  = ReadInt("Room" ,"KeepliveTime", 100);
	wincoin1 = ReadInt("Room" ,"TrumptCoin1", 50000);
	wincoin2 = ReadInt("Room" ,"TrumptCoin2", 10000);
	wincoin3 = ReadInt("Room" ,"TrumptCoin3", 5000);
	wincoin4 = ReadInt("Room", "TrumptCoin4", 2000);
	privatetime = ReadInt("Room" ,"PrivateTime", 5);


	char key[15];
	sprintf(key,"Alloc_%d",this->level);	
	//匹配服务器
	std::string alloc_host =  ReadString(key,"Host","NULL");
	strcpy( alloc_ip, alloc_host.c_str() );
    alloc_port = ReadInt(key,"Port",0);
	numplayer = ReadInt(key,"NumPlayer",9);
	rewardcoin = ReadInt(key,"RewardCoin",1000000);
	rewardroll = ReadInt(key,"RewardRoll",1000000);
	rewardRate = ReadInt(key,"RewardRate",80);

	if(alloc_port == 0 )
	{
		printf("allocinfo alloc_port is null is error level:%d\n", this->level);
		return -1;
	}

	maxmulone = ReadInt("MaxMul","MulOneRound",4);
	maxmultwo = ReadInt("MaxMul","TwoOneRound",5);
	betcointime = ReadInt("Timer","BetCoinTime",20);
	tablestarttime = ReadInt("Timer","TableStartTime",20);
	kicktime = ReadInt("Timer","KickTime",10);

	timeoutCount = ReadInt("TimeOutCount","num",2);
	fraction = ReadInt("Fraction","num",100);

	esayTaskCount = ReadInt("EsayTask","PlayCount",30);
	esayRandNum = ReadInt("EsayTask","RandNum",60);
	esayTaskRand = ReadInt("EsayTask","TaskRand",50);
	curBeforeCount = ReadInt("EsayTask","curBeforeCount",10);
	esayTaskProbability = ReadInt("EsayTask","esayTaskProbability",80);
	getIngotNoti1 = ReadInt("Notify","Num1",10);
	getIngotNoti2 = ReadInt("Notify","Num2",10);
	getIngotNoti3 = ReadInt("Notify","Num3",10);

	robotTabNum1 = ReadInt("RobotTab","Num1",10);
	robotTabNum2 = ReadInt("RobotTab","Num2",10);
	robotTabNum6 = ReadInt("RobotTab","Num6",10);

	return true;
}



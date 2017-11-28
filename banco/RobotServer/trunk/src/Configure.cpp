#include "Configure.h"
#include "Logger.h"
#include "Protocol.h"

Configure::Configure() : GameConfig(E_BACCARAT_GAME_ID, E_MSG_SOURCE_ROBOT, E_CONFIG_ROBOT)
{
	basebettime = 0;
	level = 0;
	m_robotId = 0;
}

bool Configure::LoadGameConfig() 
{
	level = m_nLevel;
	m_robotId = m_nRobotIndex;

	monitortime = ReadInt("Timer","MonitorTimer",10);
	basebettime = ReadInt("Timer","BaseBetTimer",2);

	baseplayCount = ReadInt("PlayCount","num",2);

	entertime = ReadInt("Coming","entertime",5);

	bankerbase = ReadInt("Coin","bankerbase",1000000);
	bankerrand = ReadInt("Coin","bankerrand",100000);
	playerbase = ReadInt("Coin","playerbase",1000000);
	playerrand = ReadInt("Coin","playerrand",10000);
	bankerwincount = ReadInt("Coin","bankerwincount",100000);

	playerplaybase = ReadInt("PlayNum","playerplaybase",3);
	playerplayrand = ReadInt("PlayNum","playerplayrand",3);
	betplaybase = ReadInt("PlayNum","betplaybase",3);
	betplayrand = ReadInt("PlayNum","betplayrand",3);
	bankerplaybase = ReadInt("PlayNum","bankerplaybase",3);
	bankerplayrand = ReadInt("PlayNum","bankerplayrand",3);
	bankerhasnum = ReadInt("PlayNum","bankerhasnum",4);

	baseuid = ReadInt("RandUID","basenum",100000);
	randuid = ReadInt("RandUID","num",10000);
	
#ifdef CRYPT
	const char* sectionHall = "CryptHall";
#else
	const char* sectionHall = "Hall";
#endif
	std::string conn_hall_host =  ReadString(sectionHall,"Host","NULL");
	strcpy( hall_ip, conn_hall_host.c_str() );
    hall_port = ReadInt(sectionHall,"Port",0);
	if(hall_port == 0)
	{
		printf("hall_port is null please check your conf\n");
		return false;
	}

	serverxml = ReadString("server.xml", "content", "");

	return true;
}

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
#include <stdlib.h>
#endif

Configure::Configure() : GameConfig(GAME_TYPE, E_MSG_SOURCE_ROBOT, E_CONFIG_ROBOT)
{
	basebettime = 0;
}

Configure::~Configure()
{
}

bool Configure::LoadGameConfig()
{
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

	serverxml = ReadString("server.hundred.xml", "content", "");
#ifdef CRYPT
	std::string conn_hall_host = ReadString("CryptHall", "Host", "NULL");
	strcpy(hall_ip, conn_hall_host.c_str());
	hall_port = ReadInt("CryptHall", "Port", 0);
#else
	std::string conn_hall_host = ReadString("Hall", "Host", "NULL");
	strcpy(hall_ip, conn_hall_host.c_str());
	hall_port = ReadInt("Hall", "Port", 0);
#endif

	return true;
}

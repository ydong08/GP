

#pragma once


#include <string>

struct GameServerInfo
{
	int svrid;
	std::string ip;
	int port;
	short level;
	short status;
	short priority;
	int maxuser;
	int maxtab;
	int curruser;
	int currtab;
	int lasttime;
	int iphoneuser;
	int androiduser;
	int ipaduser;
	int rotobuser;
	int gameid;
};
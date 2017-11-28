#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_

#include <time.h>
#include <unistd.h>
#include <string>
#include "Typedef.h"
#include "Game/GameConfig.h"
 
class Configure :public GameConfig
{
	Singleton(Configure);

public:
	bool LoadGameConfig();

public:
	UINT16	      m_robotId;	//第几个机器人服务器

	//连接大厅的ip和端口
	std::string   m_hallIp;
	short		  m_hallPort;
	//头像链接
	std::string   m_headLink;
	
	int baseMoney1;
	int baseMoney2;
	int baseMoney3;
	int baseMoney4;

	int randMoney1;
	int randMoney2;
	int randMoney3;
	int randMoney4;

	int swtichWin1;
	int swtichWin2;
	int swtichWin3;
	int swtichWin4;

	std::string serverxml;
}; 
#endif

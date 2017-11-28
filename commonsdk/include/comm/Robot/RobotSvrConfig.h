#ifndef __SVR_CONFIG_H__
#define __SVR_CONFIG_H__
#include <string.h>
#include "Typedef.h"

#define MAX_SERVER_NODE_LIST   (128)

class ServerNode
{
public:
	ServerNode(int svid, const char* ip, short port)
	{
		this->svid = svid;
		strcpy(this->ip,ip);
		this->port = port;
	}
	char  ip[64];		
	short port;
	int   svid;
	char  des[128];
};

class RobotSvrConfig
{
	Singleton(RobotSvrConfig);

public:	
	void initServerConf();
	int initServerConf(const char* content);

	int getServerListSize(){return serversize;};
		
	ServerNode* getServerNode(int i)
	{
		return serverlists[i];
	}

private:
	int parseServerXML(const char* path);

private:
	ServerNode* serverlists[MAX_SERVER_NODE_LIST];
	int			serversize;
};

#endif

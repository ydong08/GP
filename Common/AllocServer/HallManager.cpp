#include <stdlib.h>
#include <string.h>
#include <map>
#include "ClientHandler.h"
//#include "ServerManager.h"
#include "HallManager.h"
//#include "RedisConnector.h"
//#include "GameCmd.h"
#include "Logger.h"
#include "ClientHandler.h"

#define USER_EXPIRE_TIME 3600*3

using namespace std;

static HallManager* instance = NULL;

HallManager* HallManager::getInstance()
{
	if(instance==NULL)
	{
		instance = new HallManager();
	}
	return instance;
}

int HallManager::addHallHandler(int hallid,ClientHandler* hall)
{
	hall->setID(hallid);
	hallMap[hallid] = hall;
	return 0;
}

void  HallManager::delHallHandler(int hallid)
{
	//_LOG_DEBUG_("erase Hall id:%d\n", hallid);
	hallMap.erase(hallid);
}

ClientHandler*  HallManager::getHallHandler(int hallid)
{
	map<int , ClientHandler*>::iterator it = hallMap.find(hallid);
	 if(it == hallMap.end())
		 return NULL;
	 else
		 return it->second;
}

int HallManager::sendToHall(int hallid,  OutputPacket* outPacket, bool isEncrypt )
{
	 ClientHandler*  hall = getHallHandler(hallid);
	 if(hall && outPacket)
	 {
		if(isEncrypt)
            outPacket->EncryptBuffer();
        return hall->Send(outPacket->packet_buf(),outPacket->packet_size());
	 }
	 else
		return -1;
}

int HallManager::sendAllHall(OutputPacket* outPacket, bool isEncrypt)
{
	//_LOG_DEBUG_("hallMap size:%d\n", hallMap.size());
	map<int , ClientHandler*>::iterator it = hallMap.begin();
	while(it != hallMap.end())
	{
		ClientHandler*  hall = it->second;
		it++;
		if(hall && outPacket)
		{
			//_LOG_DEBUG_("Hall id:%d\n", hall->getID());
			if(isEncrypt)
				outPacket->EncryptBuffer();
			hall->Send(outPacket->packet_buf(),outPacket->packet_size());
		}
	}
	return 0;
}


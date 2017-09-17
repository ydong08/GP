#include <stdlib.h>
#include <string.h>
#include "HallManager.h"
#include "ClientHandler.h"
#include "Packet.h"
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

int HallManager::sendToHall(int hallid,  OutputPacket* outPacket, bool isEncrypt)
{
	 ClientHandler*  hall = getHallHandler(hallid);
	 if(hall && outPacket)
	 {
		return hall->Send(outPacket, isEncrypt);
	 }
	 else
		return -1;
	return 0;
}

int HallManager::addHallHandler(int hallid,ClientHandler* hall)
{
	hall->setID(hallid);
	hallMap[hallid] = hall;
	return 0;
}

ClientHandler*  HallManager::getHallHandler(int hallid)
{
	map<int , ClientHandler*>::iterator it = hallMap.find(hallid);
	 if(it == hallMap.end())
		 return NULL;
	 else
		 return it->second;
}

void  HallManager::delHallHandler(int hallid)
{
	hallMap.erase(hallid);
}


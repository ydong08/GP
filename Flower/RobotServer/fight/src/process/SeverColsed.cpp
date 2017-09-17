#include "SeverColsed.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include <string>
using namespace std;

SeverColsed::SeverColsed()
{
	this->name = "SeverColsed";
}

SeverColsed::~SeverColsed()
{

} 

int SeverColsed::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int SeverColsed::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = dynamic_cast<HallHandler*>(clientHandler);
	if (hallHandler == NULL)
	{
		return EXIT;
	}
	LOGGER(E_LOG_DEBUG) << "[robotexit] uid=" << hallHandler->uid;
	return EXIT;
}

REGISTER_PROCESS(SERVER_GAMESERVER_CLOSED, SeverColsed)


#include "LeaveProcess.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

REGISTER_PROCESS(CLIENT_MSG_LEAVE, LeaveProcess)

LeaveProcess::LeaveProcess()
{
	this->name = "LeaveProcess";
}

LeaveProcess::~LeaveProcess()
{

} 

int LeaveProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if (player == NULL)
	{
		LOGGER(E_LOG_DEBUG) << "player " << clientHandler->uid << "is not find";
		return 0;
	}
	
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_LEAVE, player->m_Uid);
	requestPacket.WriteInt(player->m_Uid);
	requestPacket.WriteInt(player->m_TableId);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	return 0;
}

int LeaveProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	
	if (retcode < 0)
	{
		ULOGGER(E_LOG_DEBUG, hallHandler->uid) << "error, retcode " << retcode;
		return EXIT;
	}
	
	if (player == NULL)
	{
		LOGGER(E_LOG_DEBUG) << "player " << hallHandler->uid << " is not find";
		return EXIT;
	}

	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int leaver = inputPacket->ReadInt();
	if (uid == leaver)
	{
		LOGGER(E_LOG_DEBUG) << "player " << uid << " is leave now";
		return EXIT;
	}
		
	if (tid != player->m_TableId)
	{
		LOGGER(E_LOG_DEBUG) << "player table id " << tid << " , current table id " << player->m_TableId;
		return EXIT;
	}

	return 0;
}


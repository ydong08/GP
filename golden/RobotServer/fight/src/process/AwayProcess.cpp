#include "AwayProcess.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

AwayProcess::AwayProcess()
{
	this->name = "AllinProcess";
}

AwayProcess::~AwayProcess()
{

} 

int AwayProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{	
	return 0;
}

int AwayProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = dynamic_cast<HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	short  retcode = inputPacket->ReadShort();
	string retmsg  = inputPacket->ReadString();
	int    uid     = inputPacket->ReadInt();
	int    tid     = inputPacket->ReadInt();
	int    awayuid = inputPacket->ReadInt();
	int8_t isaway  = inputPacket->ReadByte();
	int    awaytime= inputPacket->ReadInt();

	if(retcode < 0 || player == NULL || uid != hallHandler->uid)
	{
		ULOGGER(E_LOG_WARNING, uid) << "[robot exit] remote uid=" << uid << " local uid=" << hallHandler->uid;
		return EXIT;
	}

	if(tid != player->tid)
	{
		ULOGGER(E_LOG_WARNING, uid) << "[robot exit] remote uid=" << uid << " local tid=" << player->tid << " tid=" << tid;
		return EXIT;
	}
	
	if (isaway == 1)  //Àë¿ª
	{ 
		if (!player->m_vAwayUid.empty()) 
		{
			player->m_vAwayUid.push_back(awayuid);
			ULOGGER(E_LOG_INFO, uid) << "AwayProcess remote uid=" << uid << " local tid=" << player->tid << " away uid=" << awayuid;
			return 0;
		}

		if (player->m_bBetCoinTimer) {
			ULOGGER(E_LOG_INFO, uid) << "stop betcoin timer";
			player->stopBetCoinTimer();
		}

		if (player->m_bCheckTimer) {
			ULOGGER(E_LOG_INFO, uid) << "stop check timer";
			player->stopCheckTimer();
		}
		player->m_vAwayUid.push_back(awayuid);
		ULOGGER(E_LOG_INFO, uid) << "push awayuid=" << awayuid;
	}
	else
	{
		ULOGGER(E_LOG_INFO, uid) << "push awayuid=" << awayuid << " iscoming";
		std::vector<int>::iterator itor = player->m_vAwayUid.begin();
		for (; itor != player->m_vAwayUid.end(); ++itor) {
			if (*itor == awayuid) {
				ULOGGER(E_LOG_INFO, uid) << "push awayuid=" << awayuid << " found it";
				player->m_vAwayUid.erase(itor);
				break;
			}
		}
		
		if (player->m_vAwayUid.empty()) 
		{
			if (player->m_bBetCoinTimer) {	
				ULOGGER(E_LOG_INFO, uid) << "restart betcoin.";
				player->startBetCoinTimer(player->id, player->m_betCoinTime);
			}
			
			if (player->m_bCheckTimer) {
				ULOGGER(E_LOG_INFO, uid) << "restart check.";
				player->startCheckTimer(player->id, player->m_checkTime);
			}
		}
	}
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_AWAY, AwayProcess)
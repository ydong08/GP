#include "KickOutPlayerProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

KickOutPlayerProc::KickOutPlayerProc()
{
	this->name = "KickOutPlayerProc";
}

KickOutPlayerProc::~KickOutPlayerProc()
{

} 

int KickOutPlayerProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int KickOutPlayerProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = dynamic_cast<HallHandler*>(clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if (retcode < 0 || player == NULL)
	{	
		ULOGGER(E_LOG_WARNING, hallHandler->uid) << "[robot exit] retcode=" << retcode << " retmsg=" << retmsg;
		return EXIT;
	}

	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int leaver = inputPacket->ReadInt();
	ULOGGER(E_LOG_INFO, uid) << "push leave uid=" << leaver;
	
	if (!player->m_vAwayUid.empty()) {
		std::vector<int>::iterator itor = player->m_vAwayUid.begin();
		for (; itor != player->m_vAwayUid.end(); ++itor) {
			if (*itor == leaver) {
				ULOGGER(E_LOG_INFO, uid) << "push awayuid=" << leaver << " found it.";
				player->m_vAwayUid.erase(itor);
				break;
			}
		}

		if (player->m_vAwayUid.empty()) 
		{
			if (player->m_bBetCoinTimer) {
				ULOGGER(E_LOG_INFO, uid) << "restart betcoin";
				player->startBetCoinTimer(player->id, player->m_betCoinTime);
			}

			if (player->m_bCheckTimer) {
				ULOGGER(E_LOG_INFO, uid) << "restart check";
				player->startCheckTimer(player->id, player->m_checkTime);
			}
		}
	}
	
	if (uid == leaver)
	{
		ULOGGER(E_LOG_INFO, uid) << "be kick out";
		return EXIT;
	}
	return 0;
}

REGISTER_PROCESS(SERVER_MSG_KICKOUT, KickOutPlayerProc)

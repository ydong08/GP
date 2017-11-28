#include "KickOutPlayerProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include <string>
using namespace std;
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(SERVER_MSG_KICKOUT, KickOutPlayerProc);

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
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv KickOutPlayerProc Packet From Server\n");
	printf("Data Recv: retcode=[%d]\n",retcode);
	printf("Data Recv: retmsg=[%s]\n",retmsg.c_str());*/
	if(retcode < 0 || player == NULL)
	{
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int leaver = inputPacket->ReadInt();
	if(uid == leaver)
		return EXIT;
	return 0;
	//return EXIT;
}



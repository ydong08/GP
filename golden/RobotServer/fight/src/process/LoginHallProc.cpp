#include "LoginHallProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Despatch.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

LoginHallProc::LoginHallProc()
{
	this->name = "LoginHallProc";
}

LoginHallProc::~LoginHallProc()
{

} 

int LoginHallProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = dynamic_cast<HallHandler*> (client);
	Player* player = PlayerManager::getInstance().getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(HALL_USER_LOGIN, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteString(player->name);
	requestPacket.WriteInt64(player->money);
	requestPacket.WriteString("{\"robot\":2}\n");
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	ULOGGER(E_LOG_DEBUG, player->id) << "name=" << player->name << " money=" << player->money;
	return 0;
}

int LoginHallProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	ULOGGER(E_LOG_DEBUG, inputPacket->GetUid()) << "retcode=" << retcode << " retmsg=" << retmsg;
	return CLIENT_MSG_LOGINCOMING;
}

REGISTER_PROCESS(HALL_USER_LOGIN, LoginHallProc)
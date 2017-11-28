#include "GetFreePlayer.h"
#include "ProtocolServerId.h"
#include "Room.h"
#include "Logger.h"
#include "ProcessManager.h"

int GetFreePlayer::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();

	if(Room::getInstance()->getStatus()==-1)
	{
		return sendErrorMsg(clientHandler, cmd, 0, -1, "room.Status=-1, Server Wait For Stop") ;
	}

	return 0;
}

REGISTER_PROCESS(ADMIN_GET_FREE_PLAYER, GetFreePlayer)

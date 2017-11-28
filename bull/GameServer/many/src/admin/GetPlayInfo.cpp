#include "GetPlayInfo.h"
#include "GameCmd.h"
#include "Room.h"
#include "Logger.h"
#include "ProcessManager.h"

GetPlayInfo::GetPlayInfo()
{
	this->name = "GetPlayInfo";
}

GetPlayInfo::~GetPlayInfo()
{
} 

int GetPlayInfo::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();

	_LOG_DEBUG_("==>[GetPlayInfo] [0x%04x] \n", cmd);
	Room* room = Room::getInstance();

	if(room->getStatus()==-1)
	{
		return sendErrorMsg(clientHandler, cmd, 0, -1, "room.m_nStatus=-1, Server Wait For Stop") ;
	}

	time_t t;       
    time(&t);               
    /*struct tm* tp= */localtime(&t);

	Table* table = room->getTable();

	int num = 0;
	int robotNotInBanklistnum = 0;
	int robotInBanklistnum = 0;
	int winRobotnum = 0;
	int banklistcount = table->BankerList.size();
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		Player* player = table->player_array[i];
		if(num == table->m_nCountPlayer)
			break;
		if(player)
		{
			if(player->source == 30 || player->source == 31)
			{
				if(player->isbankerlist)
					robotInBanklistnum++;
				else
					robotNotInBanklistnum++;
			}
			if(player->source == 31)
				winRobotnum++;
			num++;
		}
	}

	OutputPacket response;
	response.Begin(cmd);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteInt(table->id);
	response.WriteShort(table->m_nStatus);
	response.WriteShort(table->m_nCountPlayer);
	response.WriteShort(banklistcount);
	response.WriteShort(robotNotInBanklistnum);
	response.WriteShort(robotInBanklistnum);
	response.WriteShort(winRobotnum);
	response.End();
	_LOG_DEBUG_("[GetPlayInfo] m_nCountPlayer:%d banklistcount[%d] robotNotInBanklistnum[%d] robotInBanklistnum[%d] winRobotnum[%d]\n",
		table->m_nCountPlayer, banklistcount, robotNotInBanklistnum, robotInBanklistnum, winRobotnum);
	this->send(clientHandler,&response);
	return 0;
}

REGISTER_PROCESS(ADMIN_GET_PLAY_INFO, GetPlayInfo)
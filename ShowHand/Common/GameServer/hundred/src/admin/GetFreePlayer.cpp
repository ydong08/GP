#include <string>

#include "GetFreePlayer.h"
#include "GameCmd.h"
#include "Room.h"
#include "Logger.h"
#include "BaseClientHandler.h"
#include "ProcessManager.h"
#include "ProtocolServerId.h"

REGISTER_PROCESS(ADMIN_GET_FREE_PLAYER, GetFreePlayer)

using namespace std;

int GetFreePlayer::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();

	//_LOG_DEBUG_("==>[GetFreePlayer] [0x%04x] \n", cmd);
	Room* room = Room::getInstance();

	if(room->getStatus()==-1)
	{
		return sendErrorMsg((BaseClientHandler*)clientHandler, -1, "room.Status=-1, Server Wait For Stop") ;
	}

	/*int total= room->getMaxTableCount(); //×Ü¼ÇÂ¼Êý
	Table* tables = room->getAllTables();

	Table* tableList[1024];
	int size = 0;
	for(int i=0; i<total; ++i )
	{
		Table* table = &tables[i];
		if(table->hasOnePlayer() || table->hasTwoPlayer())
		{	
			for(int j = 0; j < GAME_PLAYER; ++j)
			{
				if(table->player_array[j] && table->player_array[j]->isonline && (table->player_array[j]->isReady() || table->player_array[j]->isComming()) && table->player_array[j]->source != 30)
				{
					table->m_nRePlayTime = table->player_array[j]->getRePlayeTime();
					tableList[size++] = table;
					break;
				}
			}
		}
	}

	OutputPacket response;
	response.Begin(ADMIN_GET_FREE_PLAYER);
	response.WriteShort(0);
	response.WriteString("");

	time_t t;
	time(&t);
	response.WriteInt(t);
	response.WriteShort(size);
	
	for(int i=0; i<size; ++i )
	{
		Table* table =  tableList[i];
		int svid = Configure::getInstance()->m_nServerId;
		int tid = (svid << 16)|table->id;
		response.WriteInt(tid);
		response.WriteShort(Configure::getInstance()->m_nLevel);
		response.WriteShort(table->m_nCountPlayer);
		response.WriteShort(table->m_nStatus);
		response.WriteInt(table->m_nRePlayTime);
	}
	response.End();

	//_LOG_DEBUG_("<==Free Player Count=[%d] \n", size);
	this->send(clientHandler,&response);*/
	return 0;
}


int GetFreePlayer::doResponse(CDLSocketHandler* client, InputPacket* inputPacket,Context* pt)
{
	_NOTUSED(client);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);	
	return 0;	
}  


int GetFreePlayer::sendErrorMsg(BaseClientHandler* clientHandler, short errcode, const char* errmsg)
{
	OutputPacket response;
	response.Begin(ADMIN_GET_FREE_PLAYER);
	response.WriteShort(errcode);
	response.WriteString(errmsg);
	response.End();
	_LOG_DEBUG_("[DATA Reponse] erron=[%d] \n", errcode);
	_LOG_DEBUG_("[DATA Reponse] errmsg=[%s] \n", errmsg);
	this->send(clientHandler,&response);
	return 0;
}

 

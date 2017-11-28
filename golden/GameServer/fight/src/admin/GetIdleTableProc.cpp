#include <string>

#include "GetIdleTableProc.h"
#include "ProtocolServerId.h"
#include "Logger.h"
#include "Room.h"
#include "ProcessManager.h"
#include "GameApp.h"

REGISTER_PROCESS(ADMIN_GET_IDLE_TABLE, GetIdleTableProc)

int GetIdleTableProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt )
{
	short cmd = inputPacket->GetCmdType();

	_LOG_DEBUG_("==>[GetIdleTableProc] [0x%04x] \n", cmd);
	Room* room = Room::getInstance();

	if(room->getStatus()==-1)
	{
		return sendErrorMsg(clientHandler, -1, "room.Status=-1, Server Wait For Stop") ;
	}

	int total= 64; //总记录数
	std::vector<Table*> tables = room->getAllTables();
	int loadtype = Configure::getInstance()->m_nLevel;

	Table* tableList[64];
	int size = 0;
	for(int i=0; i<total; ++i )
	{
		Table* table = tables[i];
		if(loadtype == 1)
		{
			if(table->isEmpty() && table->id < Configure::getInstance()->robotTabNum1)
			{
				if(size < 8)
					tableList[size++] = table;
			}
		}
		else if (loadtype == 2)
		{
			if(table->isEmpty() && table->id < Configure::getInstance()->robotTabNum2)
			{
				if(size < 8)
					tableList[size++] = table;
			}
		}

		if(table->isNotFull() && !table->isLock() && !table->isEmpty())
		{
			for(int j = 0; j < table->m_bMaxNum; ++j)
			{
				if(table->player_array[j] && !table->player_array[j]->isdropline && table->player_array[j]->source != 30)
				{
					table->m_nRePlayTime = table->player_array[j]->getRePlayeTime();
					tableList[size++] = table;
					break;
				}
			}
		}

		if(table->hasOnePlayer())
		{
			for(int j = 0; j < GAME_PLAYER; ++j)
			{
				if(table->player_array[j] && table->player_array[j]->source == 30)
				{
					table->m_nRePlayTime = table->player_array[j]->getRePlayeTime();
					tableList[size++] = table;
					break;
				}
			}
		}
	}

	OutputPacket response;
	response.Begin(ADMIN_GET_IDLE_TABLE);
	response.WriteShort(0);
	response.WriteString("");

	response.WriteInt(time(NULL));
	response.WriteShort(size);
	
	for(int i=0; i<size; ++i )
	{
		Table* table = tableList[i];
		int svid = Configure::getInstance()->m_nServerId;
		int tid = (svid << 16)|table->id;
		response.WriteInt(tid);
		response.WriteShort(table->m_nStatus);
		response.WriteShort(Configure::getInstance()->m_nLevel);
		response.WriteShort(table->m_nCountPlayer);
		response.WriteInt(table->m_nRePlayTime);
	}
	response.End();

	_LOG_DEBUG_("<==Free Table Count=[%d] \n", size);
	this->send(clientHandler,&response);
	return 0;
}

int GetIdleTableProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
		
	return 0;	
}  

int GetIdleTableProc::sendErrorMsg(CDLSocketHandler* clientHandler, short errcode, const char* errmsg)
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




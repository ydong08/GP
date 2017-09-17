#include <string>

#include "GetFreePlayer.h"
#include "../GameCmd.h"
#include "../model/Room.h"
#include "Logger.h"
using namespace std;

int GetFreePlayer::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();

	//_LOG_DEBUG_("==>[GetFreePlayer] [0x%04x] \n", cmd);
	Room* room = Room::getInstance();

	if(room->getStatus()==-1)
	{
		return sendErrorMsg((ClientHandler*)clientHandler, -1, "room.Status=-1, Server Wait For Stop") ;
	}

	int total= room->getMaxTableCount(); //总记录数
	Table* tables = room->getAllTables();

	Player* playerList[1024];
	int size = 0;
	for(int i=0; i<total; ++i )
	{
		Table* table = &tables[i];
		if(table->hasOnePlayer())
		{	
			for(int j = 0; j < GAME_PLAYER; ++j)
			{
				if(table->player_array[j] && !table->player_array[j]->isdropline && (table->player_array[j]->isReady() || table->player_array[j]->isComming()) && table->player_array[j]->source != 30)
				{
					playerList[size++] = table->player_array[j];
					break;
				}
			}
		}

		if(table->isNotFull() && table->isActive())
		{
			for(int j = 0; j < GAME_PLAYER; ++j)
			{
				//如果当前房间里面有真实用户则算房间没有满
				if(table->player_array[j] && !table->player_array[j]->isdropline && table->player_array[j]->source != 30)
				{
					playerList[size++] = table->player_array[j];
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
		Player* player =  playerList[i];
		int svid = Configure::getInstance()->server_id;
		int tid = (svid << 16)|player->tid;
		response.WriteInt(player->id);
		response.WriteInt(tid);
		response.WriteShort(Configure::getInstance()->level);
		response.WriteInt64(player->carrycoin);
		response.WriteShort(player->source);
		response.WriteShort(player->status);
		response.WriteInt(player->getRePlayeTime());
	}
	response.End();

	//_LOG_DEBUG_("<==Free Player Count=[%d] \n", size);
	this->send(clientHandler,&response);
	return 0;
}

int GetFreePlayer::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
		
	return 0;	
}  

int GetFreePlayer::sendErrorMsg(ClientHandler* clientHandler, short errcode, const char* errmsg) 
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

 

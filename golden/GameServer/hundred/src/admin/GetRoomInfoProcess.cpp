#include "GetRoomInfoProcess.h"
#include "ProtocolServerId.h"
#include "Room.h"
#include "Logger.h"
#include "ProcessManager.h"

REGISTER_PROCESS(ADMIN_MSG_ROOMINFO, GetRoomInfoProcess)


int GetRoomInfoProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();
	short subcmd = inputPacket->GetSubCmd();
	short PageNo = inputPacket->ReadShort();
	short PageNum = inputPacket->ReadShort();

	_LOG_DEBUG_("==>[GetRoomInfoProcess] [0x%04x] \n", cmd);
	_LOG_DEBUG_("SubCmd=[%d] \n", subcmd);
	_LOG_DEBUG_("[DATA Parse] PageNo=[%d] \n", PageNo);
	_LOG_DEBUG_("[DATA Parse] PageNum=[%d] \n", PageNum);
		
	return doGetRoomInfo(clientHandler,PageNo, PageNum);
}

int GetRoomInfoProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{	
	return 0;
}

int GetRoomInfoProcess::doGetRoomInfo(CDLSocketHandler* clientHandler,int PageNo, int PageNum)
{
	Room* room = Room::getInstance();
	int total= room->getMaxTableCount(); //总记录数
	vector<Table*> tables = room->getAllTables();

	int start = (PageNo-1)*PageNum; //起始位置
	int end= start + PageNum;           //结束位置
		
	if( start<0 )  start = 0;
	if(end<0) end = 0;
	else if( end > total) end = total;

	int count = end - start;

	OutputPacket response;
	response.Begin(ADMIN_MSG_ROOMINFO);
	response.WriteShort(0);
	response.WriteString("");
	response.WriteShort(total);
	response.WriteShort(count);

	 

	for(int i=start; i<end; ++i)
	{
		Table* table = tables[i];
		response.WriteShort(table->id);
		response.WriteShort(table->m_nStatus);
			 
		if(table->player_array[0])
		{
			response.WriteInt(table->player_array[0]->id);
			response.WriteString(table->player_array[0]->name);
			response.WriteShort(table->player_array[0]->m_nStatus);
		}
		else
		{
			response.WriteInt(0);
			response.WriteString("");
			response.WriteShort(0);
		}

		if(table->player_array[1])
		{
			response.WriteInt(table->player_array[1]->id);
			response.WriteString(table->player_array[1]->name);
			response.WriteShort(table->player_array[1]->m_nStatus);
		}
		else
		{
			response.WriteInt(0);
			response.WriteString("");
			response.WriteShort(0);
		}

		if(table->player_array[2])
		{
			response.WriteInt(table->player_array[2]->id);
			response.WriteString(table->player_array[2]->name);
			response.WriteShort(table->player_array[2]->m_nStatus);
		}
		else
		{
			response.WriteInt(0);
			response.WriteString("");
			response.WriteShort(0);
		}

		if(table->player_array[3])
		{
			response.WriteInt(table->player_array[3]->id);
			response.WriteString(table->player_array[3]->name);
			response.WriteShort(table->player_array[3]->m_nStatus);
		}

		else
		{
			response.WriteInt(0);
			response.WriteString("");
			response.WriteShort(0);
		}
	}

	response.End();
	_LOG_DEBUG_("==>[Send response length = [%d] \n",response.packet_size()); 
		
	this->send(clientHandler, &response);
	return 0;
}

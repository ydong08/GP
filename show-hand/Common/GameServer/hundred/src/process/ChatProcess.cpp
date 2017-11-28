#include "ChatProcess.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "BaseClientHandler.h"
#include <string>
#include "ProcessManager.h"
#include "GameCmd.h"

REGISTER_PROCESS(CLIENT_MSG_CHAT, ChatProcess)
using namespace std;

struct Param
{
	int uid;
	int tid;
	int touid;
	short msgtype;
};


ChatProcess::ChatProcess()
{
	this->name = "ChatProcess";
}

ChatProcess::~ChatProcess()
{

} 

int ChatProcess::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	int touid = pPacket->ReadInt();
	BYTE type = pPacket->ReadByte();
	string msg = pPacket->ReadString();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[ChatProcess]  cmd[0x%04x] uid[%d] type[%d]\n", cmd, uid, type);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d] touid[%d]\n", tid, svid, realTid, touid);

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable();

	if(table == NULL)
	{
		_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}
	Player* opponent = table->getPlayer(touid);
	if(touid != 0)
	{
		if(opponent == NULL)
		{
			_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] touid[%d] not in This Table\n",uid, tid, realTid, touid);
			return sendErrorMsg(hallhandler, cmd, uid, -29,ErrorMsg::getInstance()->getErrMsg(-29),seq);
		}
	}

	if(Configure::getInstance()->switchchat != 1)
	{
		_LOG_ERROR_("Your uid[%d] Switch Chat is Off\n", uid);
		sendErrorMsg(hallhandler, cmd, uid, -30,ErrorMsg::getInstance()->getErrMsg(-30),seq);
		return 0;
	}

	_LOG_INFO_("[ChatProcess] UID=[%d] tid=[%d] realTid=[%d] ChatProcess OK\n", uid, tid, realTid);

	player->setActiveTime(time(NULL));

	////普通聊天
	//if(type == 1)
	//{

	//	WordServerConnect* connect = WordServerConnect::getInstance();
	//	OutputPacket ReportPkg;
	//	ReportPkg.Begin(cmd,uid);
	//	ReportPkg.SetSeqNum(pt->seq);
	//	ReportPkg.WriteShort(type);
	//	ReportPkg.WriteString(msg);
	//	ReportPkg.End();
	//	pt->seq = seq;
	//	//先往屏蔽服务器发送，如果失败就不进行过滤，直接发给对手
	//	if(connect->Send(&ReportPkg)>=0)
	//	{
	//		_LOG_DEBUG_("Transfer request to Back_WordServer OK\n" );
	//		Param* param =  (struct Param *) malloc (sizeof(struct Param));;
	//		param->uid = uid;
	//		param->tid = tid;
	//		param->msgtype = type;
	//		param->touid = touid;
	//		pt->data = param;
	//		return 1;
	//	}

	//	_LOG_INFO_("WordServerConnect Send Error!\n");
	//	return 0;
	//}


	sendChatInfo(table, player, touid, type, msg.c_str(), seq);

	return 0;
}

int ChatProcess::sendChatInfo(Table* table, Player* player, int touid,  short type, const char* msg, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = (svid << 16)|table->id;
	_LOG_DEBUG_("<==[sendChatInfo] Push [0x%04x]\n", CLIENT_MSG_CHAT);
	_LOG_DEBUG_("[Data Response] err=[0], errmsg[]\n");
	int i = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(table->player_array[i])
		{
			OutputPacket response;
			response.Begin(CLIENT_MSG_CHAT,table->player_array[i]->id);
			if(table->player_array[i]->id == player->id)
				response.SetSeqNum(seq);
			response.WriteShort(0);
			response.WriteString("");
			response.WriteInt(table->player_array[i]->id);
			response.WriteShort(table->player_array[i]->m_nStatus);
			response.WriteInt(tid);
			response.WriteShort(table->m_nStatus);
			response.WriteInt(player->id);
			response.WriteInt(touid);
			response.WriteByte(type);
			response.WriteString(msg);
			response.WriteInt64(player->m_lMoney);
			response.End();
			HallManager::getInstance()->sendToHall(table->player_array[i]->m_nHallid, &response, false);
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] ChatPlayer=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] touid=[%d]\n", touid);
	_LOG_DEBUG_("[Data Response] type=[%d]\n", type);
	_LOG_DEBUG_("[Data Response] m_lMoney=[%d]\n", player->m_lMoney);
	return 0;
}

int ChatProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	if(pt==NULL)
	{
       _LOG_ERROR_("[ChatProcess:doResponse]Context is NULL\n");	
	   return -1;
	}
	if(pt->client == NULL)
	{
		_LOG_ERROR_("[ChatProcess:doResponse]Context is client NULL\n");	
	   return -1;
	}

	short cmd = inputPacket->GetCmdType() ;
	short subcmd = inputPacket->GetSubCmd();
	short type = inputPacket->ReadShort();
	string msg = inputPacket->ReadString();
	struct Param* param = (struct Param*)pt->data;
	int uid = param->uid;
	int tid = param->tid;
	int msgtype = param->msgtype;
	int touid = param->touid;

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (pt->client);

	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	Room* room = Room::getInstance();

	Table* table = room->getTable();
	if(table == NULL)
	{
		_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),pt->seq);
	}

	Player* player = table->getPlayer(uid);
	if(player == NULL)
	{
		_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n",uid, tid, realTid);
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),pt->seq);
	}
	Player* opponent = table->getPlayer(touid);
	if(touid != 0)
	{
		if(opponent == NULL)
		{
			_LOG_ERROR_("[ChatProcess] uid=[%d] tid=[%d] realTid=[%d] touid[%d] not in This Table\n",uid, tid, realTid, touid);
			return sendErrorMsg(hallhandler, cmd, uid, -29,ErrorMsg::getInstance()->getErrMsg(-29),pt->seq);
		}
	}

	sendChatInfo(table, player, touid, type, msg.c_str(), pt->seq);
	return 0;
}

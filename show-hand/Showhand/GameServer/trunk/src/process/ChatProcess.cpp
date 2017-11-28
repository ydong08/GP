#include "ChatProcess.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "WordServerConnect.h"
#include <string>
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
	
	_LOG_INFO_("==>[ChatProcess]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d] touid[%d]\n", tid, svid, realTid, touid);
	_LOG_DEBUG_("[DATA Parse] type=[%d]\n", type);

	ClientHandler* hallhandler = reinterpret_cast<ClientHandler*> (clientHandler);

	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

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
			return sendErrorMsg(hallhandler, cmd, uid, -10,ErrorMsg::getInstance()->getErrMsg(-10),seq);
		}
	}

	player->setActiveTime(time(NULL));


	_LOG_INFO_("[ChatProcess] UID=[%d] tid=[%d] realTid=[%d] type=[%d] ChatProcess OK\n", uid, tid, realTid, type);
	//普通聊天
	if(type == 1)
	{
		if(Configure::getInstance()->level == 3)
			return sendErrorMsg(hallhandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(7),seq);
		WordServerConnect* connect = WordServerConnect::getInstance();
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd,uid);
		ReportPkg.SetSeqNum(pt->seq);
		ReportPkg.WriteShort(type);
		ReportPkg.WriteString(msg);
		ReportPkg.End();
		pt->seq = seq;
		//先往屏蔽服务器发送，如果失败就不进行过滤，直接发给对手
		if(connect->Send(&ReportPkg)>=0)
		{
			_LOG_DEBUG_("Transfer request to Back_WordServer OK\n" );
			Param* param =  (struct Param *) malloc (sizeof(struct Param));;
			param->uid = uid;
			param->tid = tid;
			param->msgtype = type;
			param->touid = touid;
			pt->data = param;
			return 1;
		}

		_LOG_INFO_("WordServerConnect Send Error!\n");
		return 0;
	}
	//魔法表情
	if(type == 2 || type == 5)
	{
		/*if(player->money - table->m_nMagicCoin > player->carrycoin)
		{
			player->money -= table->m_nMagicCoin;
			player->m_nMagicNum ++;
			player->m_nMagicCoinCount += table->m_nMagicCoin;
		}
		else
		{
			if(player->money - table->m_nMagicCoin - player->betCoinList[0] < 0)
			{
				_LOG_ERROR_("[ChatProcess] UID=[%d] tid=[%d] realTid=[%d] Not enuogh Money [%ld] has betcoin[%ld]\n",uid, tid, realTid, player->money, player->betCoinList[0]);
				return sendErrorMsg(hallhandler, cmd, uid, -22,ErrorMsg::getInstance()->getErrMsg(-22),seq);
			}
			//更新最新能下最大的金币数额
			player->carrycoin = player->money - table->m_nMagicCoin;
			player->money -= table->m_nMagicCoin;
			player->m_nMagicNum ++;
			player->m_nMagicCoinCount += table->m_nMagicCoin;
		}*/

		if(player->money - table->m_nMagicCoin - player->betCoinList[0] - player->m_nMagicCoinCount < 0)
		{
			_LOG_ERROR_("[ChatProcess] UID=[%d] tid=[%d] realTid=[%d] Not enuogh Money [%ld] has betcoin[%ld]\n",uid, tid, realTid, player->money, player->betCoinList[0]);
			return sendErrorMsg(hallhandler, cmd, uid, -24,ErrorMsg::getInstance()->getErrMsg(-24),seq);
		}
		player->m_nMagicNum ++;
		player->m_nMagicCoinCount += table->m_nMagicCoin;
		_LOG_DEBUG_("m_nMagicNum:%d m_nMagicCoinCount:%d\n", player->m_nMagicNum, player->m_nMagicCoinCount);
	}

	sendChatInfo(table, player, touid, type, msg.c_str(), seq);

	return 0;
}

int ChatProcess::sendChatInfo(Table* table, Player* player, int touid,  short type, const char* msg, short seq)
{
	int svid = Configure::getInstance()->server_id;
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
			response.WriteShort(table->player_array[i]->status);
			response.WriteInt(tid);
			response.WriteShort(table->status);
			response.WriteInt(player->id);
			response.WriteInt(touid);
			response.WriteByte(type);
			response.WriteString(msg);
			if(player->isReady() || player->isComming())
				response.WriteInt64(player->money - player->m_nMagicCoinCount);
			else
				response.WriteInt64(player->money - player->m_nMagicCoinCount + table->tax);
			response.End();
			HallManager::getInstance()->sendToHall(table->player_array[i]->hallid, &response, false);
		}
	}
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] ChatPlayer=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] touid=[%d]\n", touid);
	_LOG_DEBUG_("[Data Response] type=[%d]\n", type);
	_LOG_DEBUG_("[Data Response] m_nMagicCoin=[%d]\n", table->m_nMagicCoin);
	_LOG_DEBUG_("[Data Response] money=[%ld]\n", player->money - player->m_nMagicCoinCount + table->tax);
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

	ClientHandler* hallhandler = reinterpret_cast<ClientHandler*> (pt->client);

	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);
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
			return sendErrorMsg(hallhandler, cmd, uid, -10,ErrorMsg::getInstance()->getErrMsg(-10),pt->seq);
		}
	}

	sendChatInfo(table, player, touid, type, msg.c_str(), pt->seq);
	return 0;
}

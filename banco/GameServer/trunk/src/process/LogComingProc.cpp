#include "LogComingProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "GameApp.h"
#include "MoneyAgent.h"
#include "RoundAgent.h"
#include "GameCmd.h"
#include "Protocol.h"
#include "ProcessManager.h"
#include "BaseClientHandler.h"
#include "ProtocolServerId.h"
#include "IProcess.h"

struct Param
{
	int uid;
	int tid;
	short source;
	char name[64];
	char json[1024];
};

LogComingProc::LogComingProc()
{
	this->name = "LogComingProc";
}

LogComingProc::~LogComingProc()
{

} 

int LogComingProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	string name = pPacket->ReadString();
	int tid = pPacket->ReadInt();
	short realTid = tid & 0x0000FFFF;
	int64_t m_lMoney = pPacket->ReadInt64();
	short level = pPacket->ReadShort();
	string json = pPacket->ReadString();

	LOGGER(E_LOG_INFO) << "client params, cmd = " << TOHEX(cmd)
		<< " seq = " << seq
		<< " source = " << source
		<< " uid = " << uid
		<< " name = " << name
		<< " tid = " << tid
		<< " realTid = " << realTid
		<< " m_lMoney = " << m_lMoney
		<< " level = " << level
		<< " json = " << json;

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	if(level != Configure::getInstance()->m_nLevel)
	{
		LOGGER(E_LOG_ERROR) << "Level is not match!";
		return sendErrorMsg(clientHandler, cmd, uid, -3, ERRMSG(-3), seq);
	}

	MoneyAgent* connect = MoneyServer();
	OutputPacket response;
	response.Begin(cmd, uid);
	response.SetSeqNum(pt->seq);
	response.WriteInt(uid);
	response.WriteShort(Configure::getInstance()->m_nServerId);
	response.End();

	pt->seq = seq;

	if (source == E_MSG_SOURCE_ROBOT)
		goto __ROBOT_LOGIN__;

	if (connect->Send(&response) >= 0)
	{
		LOGGER(E_LOG_INFO) << "Transfer request to Back_MYSQLServer OK!";
		struct Param* param = (struct Param *) malloc(sizeof(struct Param));
		param->uid = uid;
		param->tid = tid;
		param->source = source;
		strncpy(param->name, name.c_str(), 63);
		param->name[63] = '\0';
		//strncpy(param->password, password.c_str(), 31);
		//param->password[31] = '\0';
		strncpy(param->json, json.c_str(), 1023);
		param->json[1023] = '\0';
		pt->data = param;
		return 1;
	}
	else
	{
		LOGGER(E_LOG_ERROR) << "Send request to BackServer Error";
		return -1;
	}

__ROBOT_LOGIN__:
	Room* room = Room::getInstance();

	Table *table = room->getTable(realTid);
	if(table == NULL)
	{
		LOGGER(E_LOG_ERROR) << "This Table is NULL";
		return sendErrorMsg(clientHandler, cmd, uid, -2, ERRMSG(-2), pt->seq);
	}

	Player* player = table->isUserInTab(uid);
	if(player==NULL)
	{
		player = room->getAvailablePlayer();
		if(player == NULL)
		{
			LOGGER(E_LOG_ERROR) << "Room is full, no seat for player";
			return sendErrorMsg(clientHandler, cmd, uid, -33, ERRMSG(-33), pt->seq);
		}
		player->id = uid;
		player->m_lMoney = m_lMoney;
		player->m_nHallid = hallhandler->hallid;
		strncpy(player->name, name.c_str(), sizeof(player->name) - 1);
		player->name[sizeof(player->name) - 1] = '\0';
		strcpy(player->json, json.c_str());
		player->source = source;

		player->login();
		if(table->playerComming(player) != 0)
		{
			LOGGER(E_LOG_ERROR) << "Comming into this table failed!";
			return sendErrorMsg(clientHandler, cmd, uid, -32, ERRMSG(-32), pt->seq);
		}
	}

	player->setActiveTime(time(NULL));

	for (int idx = 0; idx < GAME_PLAYER; ++idx)
	{
		Player *current = table->player_array[idx];
		if (current == NULL)
		{
			continue;
		}

		IProcess::sendTabePlayersInfo(table, current, uid, seq);
	}

	return 0;
}

int LogComingProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	if(pt==NULL)
	{
       LOGGER(E_LOG_INFO) << "Context is NULL";	
	   return -1;
	}
	if(pt->client == NULL)
	{
		LOGGER(E_LOG_INFO) << "Context client is NULL";
	   return -1;
	}

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (pt->client);

	struct Param* param = (struct Param*)pt->data;
	int uid = param->uid;
	int tid = param->tid;
	short source = param->source;
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	string name = string(param->name);
	string json = string(param->json);

	LOGGER(E_LOG_INFO) << "context params, uid = " << uid
		<< " tid = " << tid
		<< " source = " << source
		<< " svid = " << svid
		<< " realTid = " << realTid
		<< " name = " << name
		<< " json = " << json;

	short cmd = inputPacket->GetCmdType();
	short retno = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	uid = inputPacket->ReadInt();
	int64_t money = inputPacket->ReadInt64();
	int64_t safemoney = inputPacket->ReadInt64();
	int roll = inputPacket->ReadInt();
	int roll1 = inputPacket->ReadInt();
	int coin = inputPacket->ReadInt();
	int exp = inputPacket->ReadInt();

	LOGGER(E_LOG_INFO) << "packet params, cmd = " << TOHEX(cmd)
		<< " retno = " << retno
		<< " retmsg = " << retmsg
		<< " money = " << money
		<< " safemoney = " << safemoney
		<< " roll = " << roll
		<< " roll1 = " << roll1
		<< " coin = " << coin
		<< " exp = " << exp;

	if (retno != 0)
	{
		return sendErrorMsg(pt->client, cmd, uid, -1, retmsg.c_str(), pt->seq);
	}

	Table *table = Room::getInstance()->getTable(realTid);
	if(table == NULL)
	{
		LOGGER(E_LOG_ERROR) << "This Table is NULL";
		return sendErrorMsg(pt->client, cmd, uid, -2, ERRMSG(-2), pt->seq);
	}

	Player* player = table->isUserInTab(uid);
	if(player==NULL)
	{
		player = Room::getInstance()->getAvailablePlayer();
		if(player == NULL)
		{
			LOGGER(E_LOG_ERROR) << "Room is full, no seat for player";
			return sendErrorMsg(pt->client, cmd, uid, -33, ERRMSG(-33), pt->seq);
		}

		player->id = uid;
		player->m_lMoney = money;
		player->m_nHallid = hallhandler->hallid;
		strncpy(player->name, name.c_str(), sizeof(player->name) - 1);
		player->name[sizeof(player->name) - 1] = '\0';
		strcpy(player->json, json.c_str());
		player->source = source;
		player->m_nRoll = roll + roll1;
		player->m_nExp = exp;

		player->login();

		if(table->playerComming(player) != 0)
		{
			LOGGER(E_LOG_ERROR) << "Comming into this table failed!";
			return sendErrorMsg(pt->client, cmd, uid, -32,ErrorMsg::getInstance()->getErrMsg(-32),pt->seq);
		}
	}

	player->setActiveTime(time(NULL));

	for (int idx = 0; idx < GAME_PLAYER; ++idx)
	{
		Player *current = table->player_array[idx];
		if (current == NULL)
		{
			continue;
		}

		IProcess::sendTabePlayersInfo(table, current, uid, pt->seq);
	}

	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_LOGINCOMING, LogComingProc);

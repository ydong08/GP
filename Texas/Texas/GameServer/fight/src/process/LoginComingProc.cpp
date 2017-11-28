#include <string>
#include "LoginComingProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "ProtocolServerId.h"
#include "ProcessManager.h"
#include "BaseClientHandler.h"
#include "json/json.h"
#include "GameApp.h"

using namespace std;

struct Param
{
	int uid;
	int tid;
	short source;
	short bflag;
	char name[64];
	char json[1024];
};

LoginComingProc::LoginComingProc()
{
	this->name = "LoginComingProc" ;
}

LoginComingProc::~LoginComingProc()
{
}

int LoginComingProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	string name = pPacket->ReadString();
	int tid = pPacket->ReadInt();
	uint64_t m_lMoney = pPacket->ReadInt64();
	short level = pPacket->ReadShort();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	string json = pPacket->ReadString();
	short buyinflag = pPacket->ReadByte();

	_LOG_INFO_("==>[LoginComingProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);
	_LOG_DEBUG_("[DATA Parse] name=[%s] \n", name.c_str());
	_LOG_DEBUG_("[DATA Parse] m_lMoney=[%lu] \n", m_lMoney);
	_LOG_DEBUG_("[DATA Parse] level=[%d] \n", level);
	_LOG_DEBUG_("[DATA Parse] json=[%s] \n", json.c_str());
	_LOG_DEBUG_("[DATA Parse] buyinflag=[%d] \n", buyinflag);


	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	OutputPacket response;
	response.Begin(cmd, uid);
	response.SetSeqNum(pt->seq);
	response.WriteInt(uid);
	response.WriteShort(Configure::getInstance()->server_id);
	response.End();
	//把当前包的序列号设置为当前进程的序列号
	//pPacket->SetSeqNum(pt->seq);
	//这里要把当前这个包的seq放到回调里面，方便继续给前端发送包
	pt->seq = seq;

	if (source == 30)
		goto __ROBOT_LOGIN__;



	if (MoneyServer()->Send(&response) >= 0)
	{
		_LOG_DEBUG_("Transfer request to Back_MYSQLServer OK\n");
		struct Param* param = (struct Param *) malloc (sizeof(struct Param));;
		param->uid = uid;
		param->tid = tid;
		param->source = source;
		strncpy(param->name, name.c_str(),63);
		param->name[63] = '\0';
		//strncpy(param->password, password.c_str(), 31);
		//param->password[31] = '\0';
		strncpy(param->json, json.c_str(), 1023);
		param->json[1023] = '\0';
		param->bflag = buyinflag;
		pt->data = param;
		return 1;
	}
	else
	{
		_LOG_ERROR_("==>[LoginComingProc] [0x%04x] uid=[%d] ERROR:[%s]\n", cmd, uid, "Send request to BackServer Error");
		return -1;
	}


__ROBOT_LOGIN__:
	Room* room = Room::getInstance();

	Player* player = room->getAvailablePlayer();
	if(player==NULL)
	{
		_LOG_ERROR_("[Room is full,no seat for player] \n" );
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}

	player->id = uid;
	player->m_lMoney = m_lMoney;
	player->m_nHallid = hallhandler->hallid;
	strncpy(player->name, name.c_str(), sizeof(player->name) - 1);
	player->name[sizeof(player->name) - 1] = '\0';
	strcpy(player->json, json.c_str());
	player->m_nStatus = STATUS_PLAYER_COMING;
	player->source = source;
	player->login();
	Table* table = NULL;
	table = room->getTable(realTid);

	if(!player->checkMoney())
	{
		_LOG_ERROR_("UID[%d] m_lMoney is too small m_lMoney[%d]\n", uid, player->m_lMoney);
		return sendErrorMsg(hallhandler, cmd, uid, -4,ErrorMsg::getInstance()->getErrMsg(-4),seq);
	}

	if(table == NULL)
	{
		_LOG_WARN_("[This Table[%d] is Full] \n", realTid );
		table = room->getAvailableTable();
		if(table == NULL)
		{
			_LOG_ERROR_("[Room is full, no table for player]\n", realTid );
			return sendErrorMsg(hallhandler, cmd, uid, -26,ErrorMsg::getInstance()->getErrMsg(-26),seq);
		}
	}

	_LOG_INFO_("[LoginComingProc] UID=[%d] NorLogin OK ,m_nSeatID[%d]\n", player->id,player->m_nSeatID );

	if(table->playerComming(player) != 0)
	{
		_LOG_WARN_("Player[%d] Coming This table[%d] Error\n", player->id, table->id);
		//如果用户进入桌子失败，则重新给用户找一个桌子进入
		table = room->getAvailableTable();
		if(table!= NULL)
		{
			_LOG_WARN_("Trans Table[%d] Coming Error, Player[%d] Coming Other table[%d]\n", tid, player->id, table->id);
			if(table->playerComming(player) != 0)
				return sendErrorMsg(hallhandler, cmd, uid, -26,ErrorMsg::getInstance()->getErrMsg(-26),seq);
		}
		else
			return sendErrorMsg(hallhandler, cmd, uid, -26,ErrorMsg::getInstance()->getErrMsg(-26),seq);
	}
	player->setActiveTime(time(NULL));

	PlayerSet::iterator it = table->m_Viewers.begin();
	for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
	{
		Player *otherplayer = NULL;
		if (i < GAME_PLAYER)
			otherplayer = table->player_array[i];
		else {
			otherplayer = *it;
			it++;
		}
		if(otherplayer)
		{
			sendTabePlayersInfo(otherplayer, table, table->m_nCountPlayer, player->id, seq);
		}
	}

	return 0;
}

int LoginComingProc::sendTabePlayersInfo(Player* player, Table* table, short num, int comuid, short seq)
{
	int svid = Configure::getInstance()->server_id;
	int tid = svid << 16|table->id;
	OutputPacket response;
	response.Begin(CLIENT_MSG_LOGINCOMING, player->id);
	if(player->id == comuid)
		response.SetSeqNum(seq);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteInt(comuid);
	response.WriteShort(player->m_nSeatID);
	response.WriteByte(player->m_nTabIndex);
	response.WriteByte(num);
	_LOG_DEBUG_("<==[LoginComingProc] Push [0x%04x]\n", CLIENT_MSG_LOGINCOMING);
	_LOG_DEBUG_("[Data Response] push to uid=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] push to m_nStatus=[%d]\n", player->m_nStatus);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_nSeatID=[%d]\n", player->m_nSeatID);
	_LOG_DEBUG_("[Data Response] comuid=[%d]\n", comuid);
	_LOG_DEBUG_("[Data Response] num=[%d]\n", num);
	int sendnum = 0;
	for(int i = 0; i < table->m_bMaxNum; ++i)
	{
		if(sendnum == num)
			break;
		Player* otherplayer = table->player_array[i];
		if(otherplayer)
		{
			response.WriteInt(otherplayer->id);
			response.WriteString(otherplayer->name);
			response.WriteShort(otherplayer->m_nStatus);
			response.WriteByte(otherplayer->m_nTabIndex);
			response.WriteInt64(otherplayer->m_lMoney);
			response.WriteInt64(otherplayer->m_lCarryMoney);
			response.WriteString(otherplayer->json);
			sendnum++;
		}
	}
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[LoginComingProc] Send To Uid[%d] Error!\n", player->id);
	else
		_LOG_DEBUG_("[LoginComingProc] Send To Uid[%d] Success\n", player->id);
	return 0;
}

int LoginComingProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )
{
	_NOTUSED(clientHandler);
	if(pt==NULL)
	{
       _LOG_ERROR_("[LoginComingProc:doResponse]Context is NULL\n");
	   return -1;
	}
	if(pt->client == NULL)
	{
		_LOG_ERROR_("[LoginComingProc:doResponse]Context is client NULL\n");
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
	short buyinflag = param->bflag;

	short cmd = inputPacket->GetCmdType();
	short retno = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();

	if (retno != 0)
	{
		_LOG_ERROR_("[%s:%s:%d]Select User Score Info Error[%d]:[%s]\n", __FILE__, __FUNCTION__, __LINE__, retno, retmsg.c_str());
		sendErrorMsg(hallhandler, cmd, uid, -1, (char*)retmsg.c_str(),pt->seq);
		return -1;
	}
	uid = inputPacket->ReadInt();
	int64_t m_lMoney = inputPacket->ReadInt64();
	int64_t safeMoney = inputPacket->ReadInt64();
	int roll = inputPacket->ReadInt();
	int roll1 = inputPacket->ReadInt();
	int coin = inputPacket->ReadInt();
	int exp = inputPacket->ReadInt();

	_LOG_DEBUG_("[DATA Parse] m_lMoney=[%ld]\n", m_lMoney);
	_LOG_DEBUG_("[DATA Parse] safeMoney=[%ld]\n", safeMoney);
	_LOG_DEBUG_("[DATA Parse] roll=[%d]\n", roll);
	_LOG_DEBUG_("[DATA Parse] roll1=[%d]\n", roll1);
	_LOG_DEBUG_("[DATA Parse] coin=[%d]\n", coin);
	_LOG_DEBUG_("[DATA Parse] exp=[%d]\n", exp);

	Room* room = Room::getInstance();

	Player* player = room->getAvailablePlayer();
	if(player==NULL)
	{
		_LOG_ERROR_("[Room is full,no seat for player] \n" );
		return sendErrorMsg(hallhandler, cmd, uid, -26,ErrorMsg::getInstance()->getErrMsg(-26),pt->seq);
	}


	player->id = uid;
	player->m_lMoney = m_lMoney;
	player->m_nHallid = hallhandler->hallid;
	strncpy(player->name, name.c_str(), sizeof(player->name) - 1);
	player->name[sizeof(player->name) - 1] = '\0';
	strcpy(player->json, json.c_str());
	player->m_nStatus = STATUS_PLAYER_COMING;
	player->source = source;
	player->m_nRoll = roll + roll1;
	player->m_nBuyInFlag = buyinflag;
	player->login();
	Table* table = NULL;
	table = room->getTable(realTid);

	if(!player->checkMoney())
	{
		_LOG_ERROR_("UID[%d] m_lMoney is too small m_lMoney[%d]\n", uid, player->m_lMoney);
		sendErrorMsg(hallhandler, cmd, uid, -4,ErrorMsg::getInstance()->getErrMsg(-4),pt->seq);
		player->setEnterTime(time(NULL));
		player->leave();
		return 0;
	}

	if(table == NULL)
	{
		_LOG_WARN_("[This Table[%d] is Null] \n", realTid );
		table = room->getAvailableTable();
		if(table == NULL)
		{
			_LOG_ERROR_("[Room is full, no table for player]\n", realTid );
			return sendErrorMsg(hallhandler, cmd, uid, -26,ErrorMsg::getInstance()->getErrMsg(-26),pt->seq);
		}
	}
	else
	{
		//当前桌子被锁定就不能进入这个桌子
		if(table->isLock())
		{
			_LOG_WARN_("[This Table[%d] is Lock] \n", realTid );
			table = room->getAvailableTable();
			if(table == NULL)
			{
				_LOG_ERROR_("[Room is full, no table for player]\n", realTid );
				return sendErrorMsg(hallhandler, cmd, uid, -26,ErrorMsg::getInstance()->getErrMsg(-26),pt->seq);
			}
		}
	}

	player->setRePlayeTime(time(NULL));
	_LOG_INFO_("[LoginComingProc] UID=[%d] NorLogin OK ,m_nSeatID[%d]\n", player->id,player->m_nSeatID );

	if(table->playerComming(player) != 0)
	{
		_LOG_WARN_("Player[%d] Coming This table[%d] Error\n", player->id, table->id);
		//如果用户进入桌子失败，则重新给用户找一个桌子进入
		table = room->getAvailableTable();
		if(table!= NULL)
		{
			_LOG_WARN_("Trans Table[%d] Coming Error, Player[%d] Coming Other table[%d]\n", tid, player->id, table->id);
			if(table->playerComming(player) != 0)
				return sendErrorMsg(hallhandler, cmd, uid, -26,ErrorMsg::getInstance()->getErrMsg(-26),pt->seq);
		}
		else
			return sendErrorMsg(hallhandler, cmd, uid, -26,ErrorMsg::getInstance()->getErrMsg(-26),pt->seq);
	}

	if(table == NULL)
		return sendErrorMsg(hallhandler, cmd, uid, -26,ErrorMsg::getInstance()->getErrMsg(-26),pt->seq);

	OutputPacket response;
	response.Begin(ROUND_GET_INFO, player->id);
	response.WriteInt(player->id);
	response.WriteShort(table->id);
	response.End();
	if(player->source != 30)
	{
		if(RoundServer()->Send(&response) < 0)
			_LOG_ERROR_("[RoundServerConnect send Error uid[%d]]\n", player->id);
	}

	Json::Value data;
	data["errcode"] = 0;
	_UDP_REPORT_(player->id, cmd,"%s",data.toStyledString().c_str());

	player->setRePlayeTime(time(NULL));

	player->setActiveTime(time(NULL));
	PlayerSet::iterator it = table->m_Viewers.begin();
	for(int i = 0; i < GAME_PLAYER + (int)table->m_Viewers.size(); ++i)
	{
		Player *otherplayer = NULL;
		if (i < GAME_PLAYER)
			otherplayer = table->player_array[i];
		else {
			otherplayer = *it;
			it++;
		}
		if(otherplayer)
		{
			sendTabePlayersInfo(otherplayer, table, table->m_nCountPlayer, player->id, pt->seq);
		}
	}

	if(!table->isLock())
		//设置踢出时间
		table->setKickTimer();
	/*else
	{
		if(player->startnum == 1)
			IProcess::serverWarnPlayerKick(table, player, Configure::getInstance()->kicktime - (time(NULL)-table->KickTime));
	}*/
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_LOGINCOMING, LoginComingProc)
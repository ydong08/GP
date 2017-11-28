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
#include "CoinConf.h"

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
	int64_t m_lMoney = pPacket->ReadInt64();
	short level = pPacket->ReadShort();
	string json = pPacket->ReadString();
	
	_LOG_INFO_("==>[LogComingProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d]\n", tid);
	_LOG_DEBUG_("[DATA Parse] name=[%s] \n", name.c_str());
	_LOG_DEBUG_("[DATA Parse] m_lMoney=[%lu] \n", m_lMoney);
	_LOG_DEBUG_("[DATA Parse] json=[%s] \n", json.c_str());

    BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	if(level != Configure::getInstance()->m_nLevel && GAME_ID != E_BULL_2017)
	{
		_LOG_ERROR_("Level[%d] is not equal Servertype[%d]\n", level, Configure::getInstance()->m_nLevel);
		return sendErrorMsg(hallhandler, cmd, uid, -3,ErrorMsg::getInstance()->getErrMsg(-3),seq);
	}

	MoneyAgent* connect = MoneyServer();
	OutputPacket response;
	response.Begin(cmd, uid);
	response.SetSeqNum(pt->seq);
	response.WriteInt(uid);
	response.WriteShort(Configure::getInstance()->m_nServerId);
	response.End();

	pt->seq = seq;

	if (source == 30)
		goto __ROBOT_LOGIN__;

	if (connect->Send(&response) >= 0)
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

	Table *table = room->getTable();
	if(table == NULL)
	{
		_LOG_WARN_("[This Table is NULL] \n");
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),pt->seq);
	}

	Player* player = table->isUserInTab(uid);
	if(player==NULL)
	{
		player = room->getAvailablePlayer();
		if(player == NULL)
		{
			_LOG_ERROR_("[Room is full,no seat for player] \n" );
			return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),pt->seq);
		}

		if (!player->checkMoney(m_lMoney))
		{
			ULOGGER(E_LOG_ERROR, uid) << "Not enough money to enter, lower_money = " << CoinConf::getInstance()->getCoinCfg()->minmoney
				<< ", owned_money = " << player->m_lMoney;
			return sendErrorMsg(clientHandler, cmd, uid, -13, ERRMSG(-13), seq);
		}

		player->id = uid;
		player->m_lMoney = m_lMoney;
		player->m_nHallid = hallhandler->hallid;
		strncpy(player->name, name.c_str(), sizeof(player->name) - 1);
		player->name[sizeof(player->name) - 1] = '\0';
		strcpy(player->json, json.c_str());
		player->source = source;


		_LOG_INFO_("[LogComingProc] UID=[%d] NorLogin OK ,m_nSeatID[%d]\n", player->id,player->m_nSeatID );

		player->login();
		if(table->playerComming(player) != 0)
		{
			_LOG_WARN_("Player[%d] Coming This table[%d] Error\n", player->id, table->id);
			return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),pt->seq);
		}
	}
	player->source = E_MSG_SOURCE_ROBOT;
	player->setActiveTime(time(NULL));
	sendTabePlayersInfo(table, player, seq);

	

	return 0;
}

int LogComingProc::sendTabePlayersInfo(Table* table, Player* player, short seq)
{
	short timeout = 0;
	if (table->isIdle())
	{
		timeout = Configure::getInstance()->idletime - (time(NULL) - table->getStatusTime());
	}
	else if (table->isBet())
	{
		timeout = Configure::getInstance()->bettime - (time(NULL) - table->getStatusTime());
	}
	else if (table->isOpen())
	{
		timeout = Configure::getInstance()->opentime - (time(NULL) - table->getStatusTime());
	}
	OutputPacket response;
	response.Begin(CLIENT_MSG_LOGINCOMING, player->id);
	response.SetSeqNum(seq);
	response.WriteShort(Configure::getInstance()->m_nLevel);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(table->id);
	response.WriteShort(table->m_nStatus);
	response.WriteInt(player->id);
	response.WriteShort(table->m_nCountPlayer);
	response.WriteShort(player->m_nSeatID);
	response.WriteInt64(player->m_lMoney);
	response.WriteByte(timeout);
	response.WriteString(player->json);
	Player* banker = NULL;
	if(table->bankersid >=0)
		banker = table->player_array[table->bankersid];
	response.WriteInt(banker ? banker->id : 0);
	response.WriteShort(banker ? banker->m_nSeatID : -1);
	response.WriteInt64(banker ? banker->m_lMoney : 0);
	response.WriteString(banker ? banker->name : "");
	response.WriteString(banker ? banker->json : "");
	response.WriteByte(CoinConf::getInstance()->getCoinCfg()->bankernumplayer - table->bankernum);
	for(int i = 1; i < 5; ++i)
	{
		response.WriteInt64(table->m_lTabBetArray[i]);
	}
	response.WriteByte(10);
	response.WriteInt(table->m_nLimitCoin);
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[LoginComingProc] Send To Uid[%d] Error!\n", player->id);

	
	_LOG_DEBUG_("<==[LogComingProc] Push [0x%04x]\n", CLIENT_MSG_LOGINCOMING);
	_LOG_DEBUG_("[Data Response] push to uid=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] push to m_nStatus=[%d]\n", player->m_nStatus);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", table->id);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_nSeatID=[%d]\n", player->m_nSeatID);
	_LOG_DEBUG_("[Data Response] comuid=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] timeout=[%d]\n", timeout);
	_LOG_DEBUG_("[Data Response] banker=[%d]\n", banker ? banker->id : 0);
	_LOG_DEBUG_("[Data Response] m_nLimitCoin=[%d]\n", table->m_nLimitCoin);
	return 0;
}

int LogComingProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
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
	
	string name = string(param->name);
	string json = string(param->json);

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
	int64_t money = inputPacket->ReadInt64();
	int64_t safemoney = inputPacket->ReadInt64();
	int roll = inputPacket->ReadInt();
	int roll1 = inputPacket->ReadInt();
	int coin = inputPacket->ReadInt();
	int exp = inputPacket->ReadInt();

	_LOG_DEBUG_("[DATA Parse] money=[%ld]\n", money);
	_LOG_DEBUG_("[DATA Parse] safemoney=[%ld]\n", safemoney);
	_LOG_DEBUG_("[DATA Parse] roll=[%d]\n", roll);
	_LOG_DEBUG_("[DATA Parse] roll1=[%d]\n", roll1);
	_LOG_DEBUG_("[DATA Parse] coin=[%d]\n", coin);
	_LOG_DEBUG_("[DATA Parse] exp=[%d]\n", exp);
	Room* room = Room::getInstance();

	Table *table = room->getTable();
	if(table == NULL)
	{
		_LOG_WARN_("[This Table is NULL] \n");
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),pt->seq);
	}

	Player* player = table->isUserInTab(uid);
	if(player==NULL)
	{
		player = room->getAvailablePlayer();
		if(player == NULL)
		{
			_LOG_ERROR_("[Room is full,no seat for player] \n" );
			return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),pt->seq);
		}

		if (!player->checkMoney(money))
		{
			ULOGGER(E_LOG_ERROR, uid) << "Not enough money to enter, lower_money = " << CoinConf::getInstance()->getCoinCfg()->minmoney
				<< ", owned_money = " << player->m_lMoney;
			return sendErrorMsg(hallhandler, cmd, uid, -13, ERRMSG(-13), pt->seq);
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
		_LOG_INFO_("[LogComingProc] UID=[%d] NorLogin OK ,m_nSeatID[%d]\n", player->id,player->m_nSeatID );

		if(table->playerComming(player) != 0)
		{
			_LOG_WARN_("Player[%d] Coming This table[%d] Error\n", player->id, table->id);
			return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),pt->seq);
		}
	}

	player->setActiveTime(time(NULL));
	if(player->source != 30)
	{
		if(RoundServer()->GetRoundInfo(player->id, table->id) < 0)
			_LOG_ERROR_("[RoundServerConnect send Error uid[%d]]\n", player->id);
	}
	sendTabePlayersInfo(table, player, pt->seq);

	table->SendSeatInfo(player);
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_LOGINCOMING, LogComingProc)
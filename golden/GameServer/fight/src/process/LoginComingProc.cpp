#include <string>
#include "LoginComingProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "IProcess.h"
#include "MoneyAgent.h"
#include "RoundAgent.h"
#include "BaseClientHandler.h"
#include "ProtocolServerId.h"
#include "GameApp.h"

REGISTER_PROCESS(CLIENT_MSG_LOGINCOMING, LoginComingProc)

using namespace std;

struct Param
{
	int uid;
	int tid;
	short source;
	char name[64];
	char json[1024];
	char userip[32];
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
	string userip = "";
	if(source != 30)
		userip = pPacket->ReadString();
	
	_LOG_INFO_("==>[LoginComingProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);
	_LOG_DEBUG_("[DATA Parse] name=[%s] \n", name.c_str());
	_LOG_DEBUG_("[DATA Parse] m_lMoney=[%lu] \n", m_lMoney);
	_LOG_DEBUG_("[DATA Parse] level=[%d] \n", level);
	_LOG_DEBUG_("[DATA Parse] json=[%s] \n", json.c_str());
	_LOG_DEBUG_("[DATA Parse] userip=[%s] \n", userip.c_str());

	BaseClientHandler* hallhandler = reinterpret_cast<BaseClientHandler*> (clientHandler);

	MoneyAgent* connect = MoneyServer();
	OutputPacket response;
	response.Begin(cmd, uid);
	response.SetSeqNum(pt->seq);
	response.WriteInt(uid);
	response.WriteShort(Configure::getInstance()->m_nServerId);
	response.End();
	//把当前包的序列号设置为当前进程的序列号
	//pPacket->SetSeqNum(pt->seq);
	//这里要把当前这个包的seq放到回调里面，方便继续给前端发送包
	pt->seq = seq;

	if (source == 30)//机器人登录
		goto __ROBOT_LOGIN__;
	
	if (connect && connect->Send(&response) >= 0)
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
		strncpy(param->userip, userip.c_str(),31);
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

	Player* player = room->getAvailablePlayer();
	if(player==NULL)
	{
		_LOG_ERROR_("[Room is full,no seat for player] \n" );
		return sendErrorMsg(hallhandler, cmd, uid, -33, _EMSG_(-33),seq);
	}

	player->id = uid;
	player->m_lMoney = m_lMoney;
	player->m_nHallid = hallhandler->hallid;
	strncpy(player->name, name.c_str(), sizeof(player->name) - 1);
	player->name[sizeof(player->name) - 1] = '\0';
	strcpy(player->json, json.c_str());
	player->m_nStatus = STATUS_PLAYER_COMING;
	player->source = source;
	Table* table = NULL;
	table = room->getTable(realTid);

	if(!player->checkMoney())
	{
		_LOG_ERROR_("UID[%d] m_lMoney is too small m_lMoney[%d]\n", uid, player->m_lMoney);
		player->m_nStatus = STATUS_PLAYER_LOGOUT;
		player->tid = -1;
		player->m_nTabIndex = -1;
		return sendErrorMsg(hallhandler, cmd, uid, -4,_EMSG_(-4),seq);
	}

	if(table == NULL)
	{
		_LOG_WARN_("[This Table[%d] is Full] \n", realTid );
		table = room->getAvailableTable();
		if(table == NULL)
		{
			_LOG_ERROR_("[Room is full, no table for player]\n", realTid );
			return sendErrorMsg(hallhandler, cmd, uid, -31, _EMSG_(-31),seq);
		}
	}

	player->login();
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
				return sendErrorMsg(hallhandler, cmd, uid, -32, _EMSG_(-32), seq);
		}
		else
			return sendErrorMsg(hallhandler, cmd, uid, -31, _EMSG_(-31), seq);
	}
	player->setActiveTime(time(NULL));
	int i= 0;
	int sendnum = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
		if(table->player_array[i])
		{
			sendTabePlayersInfo(table->player_array[i], table, table->m_nCountPlayer, player->id, seq);
			sendnum++;
		}
	}

	return 0;
}

int LoginComingProc::sendTabePlayersInfo(Player* player, Table* table, short num, int comuid, short seq)
{
	int svid = Configure::getInstance()->m_nServerId;
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
	response.WriteInt(table->m_nAnte);
	response.WriteByte(num);
	_LOG_DEBUG_("<==[LoginComingProc] Push [0x%04x]\n", CLIENT_MSG_LOGINCOMING);
	_LOG_DEBUG_("[Data Response] push to uid=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] push to m_nStatus=[%d]\n", player->m_nStatus);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tm_nStatus=[%d]\n", table->m_nStatus);
	_LOG_DEBUG_("[Data Response] m_nSeatID=[%d]\n", player->m_nSeatID);
	_LOG_DEBUG_("[Data Response] comuid=[%d]\n", comuid);
	_LOG_DEBUG_("[Data Response] num=[%d]\n", num);
	for(int i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* otherplayer = table->player_array[i];
		if(otherplayer)
		{
			response.WriteInt(otherplayer->id);
			response.WriteString(otherplayer->name);
			response.WriteShort(otherplayer->m_nStatus);
			response.WriteByte(otherplayer->m_nTabIndex);
			response.WriteInt64(otherplayer->m_lMoney);
			response.WriteString(otherplayer->json);
		}
	}
	response.WriteByte(table->m_nMulCount);
	response.WriteByte(table->m_nMulNum1);
	response.WriteInt(table->m_nMulCoin1);
	response.WriteByte(table->m_nMulNum2);
	response.WriteInt(table->m_nMulCoin2);

	response.WriteByte(table->m_nForbidCount);
	response.WriteByte(table->m_nForbidNum1);
	response.WriteInt(table->m_nForbidCoin1);
	response.WriteByte(table->m_nForbidNum2);
	response.WriteInt(table->m_nForbidCoin2);

	response.WriteByte(table->m_nChangeCount);
	response.WriteInt(table->m_nChangeCoin1);
	response.WriteInt(table->m_nChangeCoin2);
	response.WriteInt(table->m_nChangeCoin3);
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
	string userip = string(param->userip);

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
	Player* player = room->getPlayerUID(uid);
	if(player)
	{
		Table* table = room->getTable(player->tid);
		if(table)
		{
			sendErrorMsg(hallhandler, cmd, uid, -2,_EMSG_(-2),pt->seq);
			_LOG_WARN_("player[%d] has in this table[%d] can't coming other table[%d]\n", player->id, player->tid, realTid);
			return 0;
		}
	}

	if (player == NULL)
		player = room->getAvailablePlayer();
	if(player==NULL)
	{
		_LOG_ERROR_("[Room is full,no seat for player] \n" );
		return sendErrorMsg(hallhandler, cmd, uid, -33, _EMSG_(-33), pt->seq);
	}
	
	player->id = uid;
	player->m_lMoney = m_lMoney;
	player->m_lTotalMoney = m_lMoney + safeMoney;
	player->m_nHallid = hallhandler->hallid;
	strncpy(player->name, name.c_str(), sizeof(player->name) - 1);
	player->name[sizeof(player->name) - 1] = '\0';
	strcpy(player->json, json.c_str());
	player->m_nStatus = STATUS_PLAYER_COMING;
	player->source = source;
	player->m_nRoll = roll + roll1;
	strcpy(player->m_byIP, userip.c_str());

	if(!player->checkMoney())
	{
		_LOG_ERROR_("UID[%d] m_lMoney is too small m_lMoney[%d]\n", uid, player->m_lMoney);
		sendErrorMsg(hallhandler, cmd, uid, -4, _EMSG_(-4), pt->seq);
		player->m_nStatus = STATUS_PLAYER_LOGOUT;
		player->tid = -1;
		player->m_nTabIndex = -1;
		return 0;
	}

	Table* table = NULL;
	table = room->getTable(realTid);
	if(table == NULL)
	{
		_LOG_WARN_("[This Table[%d] is Null] \n", realTid );
		table = room->getAvailableTable();
		if(table == NULL)
		{
			_LOG_ERROR_("[Room is full, no table for player]\n", realTid );
			return sendErrorMsg(hallhandler, cmd, uid, -31, _EMSG_(-31), pt->seq);
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
				return sendErrorMsg(hallhandler, cmd, uid, -31, _EMSG_(-31), pt->seq);
			}
		}
	}

	if (!table->CheckIP(userip))
	{
		_LOG_WARN_("[This Table[%d] has same ip.", realTid );
		table = room->getAvailableTable(uid, userip);
		if(table == NULL)
		{
			_LOG_ERROR_("[Room is full, no table for player]\n", realTid );
			return sendErrorMsg(hallhandler, cmd, uid, -31,_EMSG_(-31),pt->seq);
		}
	}

	int i = 0;
	for (i = 0; i < Room::getInstance()->getMaxTableCount(); ++i) {
		int ret = table->playerComming(player);
		if (ret == -2)  //seat full
		{
			table = room->getAvailableTable(uid, userip);
			_LOG_WARN_("Player[%d] Coming This table[%d] Seat Full.\n", player->id, table->id);
			// return sendErrorMsg(hallhandler, cmd, uid, -24,_EMSG_(-24),pt->seq);
		} else {
			break;
		}
	}
	if ((i == Room::getInstance()->getMaxTableCount()) || (table == NULL)) {
		_LOG_WARN_("findcount[%d] Player[%d] Coming This table[%d] Error\n", i, player->id, table->id);
		return sendErrorMsg(hallhandler, cmd, uid, -31, _EMSG_(-31), pt->seq);
	}

	_LOG_INFO_("[LoginComingProc] UID=[%d] NorLogin OK ,m_nSeatID[%d]\n", player->id,player->m_nSeatID, table->id);
	player->setRePlayeTime(time(NULL));
	player->m_nChangeNum = table->m_nChangeCount;
	
	if(player->source != 30)
	{
		if (RoundServer()->GetRoundInfo(player->id, table->id) < 0)
			_LOG_ERROR_("[RoundServerConnect send Error uid[%d]]\n", player->id);
	}

	player->setRePlayeTime(time(NULL));
	player->setActiveTime(time(NULL));
	int sendnum = 0;
	for(i = 0; i < table->m_bMaxNum; ++i)
	{
		if(sendnum == table->m_nCountPlayer)
			break;
		if(table->player_array[i])
		{
			sendTabePlayersInfo(table->player_array[i], table, table->m_nCountPlayer, player->id, pt->seq);
			sendnum++;
		}
	}
	
	if(!table->isLock())
		//设置踢出时间
		table->setKickTimer();
	else
	{
		if(player->startnum == 1)
			IProcess::serverWarnPlayerKick(table, player, Configure::getInstance()->kicktime - (time(NULL)-table->KickTime));
	}
	return 0;
}


int LoginComingProc::sendTableInfo(Player* player, Table* table)
{
	int svid = Configure::getInstance()->m_nServerId;
	int tid = svid << 16|table->id;
	int i= 0;
	Player* betplayer = table->getPlayer(table->m_nCurrBetUid);
	short remaintime = 0;
	//当前超时时间
	if(betplayer)
		remaintime = Configure::getInstance()->betcointime - (time(NULL) - betplayer->GetBetCoinTime());
	OutputPacket response;
	response.Begin(CLIENT_MSG_TABLEDET, player->id);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->m_nStatus);
	response.WriteInt(tid);
	response.WriteShort(table->m_nStatus);
	response.WriteByte(Configure::getInstance()->max_round);
	response.WriteInt(table->m_nAnte);
	response.WriteInt(table->m_nTax);
	if(table->player_array[table->m_nDealerIndex])
		response.WriteInt(table->player_array[table->m_nDealerIndex]->id);
	else
		response.WriteInt(0);
	if(player->m_bCardStatus == CARD_KNOWN)
	{
		response.WriteByte(player->card_array[0]);
		response.WriteByte(player->card_array[1]);
		response.WriteByte(player->card_array[2]);
	}
	else
	{
		response.WriteByte(0);
		response.WriteByte(0);
		response.WriteByte(0);
	}
	response.WriteByte(table->m_bCurrRound);
	response.WriteInt64(table->m_lCurrBetMax);
	response.WriteShort(player->optype);
	response.WriteInt64(table->m_lSumPool);
	response.WriteInt(betplayer ? betplayer->id : 0);
	response.WriteByte(remaintime>16 ? remaintime-3 : remaintime);
	response.WriteByte(Configure::getInstance()->betcointime - 3);
	if (player->m_bCardStatus == CARD_UNKNOWN)
	{
		response.WriteInt(table->m_nRase1);
		response.WriteInt(table->m_nRase2);
		response.WriteInt(table->m_nRase3);
		response.WriteInt(table->m_nRase4);
		response.WriteInt(table->m_nMaxLimit);
	}
	else
	{
		response.WriteInt(table->m_nRase2);
		response.WriteInt(table->m_nRase3);
		response.WriteInt(table->m_nRase4);
		response.WriteInt(table->m_nMaxLimit);
		response.WriteInt(table->m_nMaxLimit * 2);
	}
	response.WriteInt64(player->m_AllinCoin);
	response.WriteShort(table->m_vecBetCoin.size());
	for(size_t i = 0; i < table->m_vecBetCoin.size(); ++i)
		response.WriteInt(table->m_vecBetCoin[i]);
	response.WriteByte(table->m_nCountPlayer);
	for(size_t i = 0; i < table->m_bMaxNum; ++i)
	{
		Player* getPlayer = table->player_array[i];
		if(getPlayer)
		{
			response.WriteInt(getPlayer->id);
			response.WriteString(getPlayer->name);
			response.WriteShort(getPlayer->m_nStatus);
			response.WriteByte(getPlayer->m_nTabIndex);
			response.WriteByte(getPlayer->m_bCardStatus);
			response.WriteInt64(getPlayer->m_lMoney);
			//说明当前正在发牌倒计时中
			response.WriteInt64(player->m_lSumBetCoin);
			response.WriteInt(getPlayer->m_nWin);
			response.WriteInt(getPlayer->m_nLose);
			response.WriteString(getPlayer->json);
			response.WriteByte(getPlayer->m_nUseMulNum);
			response.WriteByte(getPlayer->m_nUseForbidNum);
			response.WriteByte(getPlayer->m_nChangeNum);
			_LOG_DEBUG_("[Data Response] getPlayerid=[%d]\n", getPlayer->id);
			_LOG_DEBUG_("[Data Response] getPlayer->m_bCardStatus=[%d]\n", getPlayer->m_bCardStatus);

		}
	}
	response.WriteShort(0);
    response.WriteString("");
	response.WriteByte(table->m_nMulCount);
	response.WriteByte(table->m_nMulNum1);
	response.WriteInt(table->m_nMulCoin1);
	response.WriteByte(table->m_nMulNum2);
	response.WriteInt(table->m_nMulCoin2);

	response.WriteByte(table->m_nForbidCount);
	response.WriteByte(table->m_nForbidNum1);
	response.WriteInt(table->m_nForbidCoin1);
	response.WriteByte(table->m_nForbidNum2);
	response.WriteInt(table->m_nForbidCoin2);

	response.WriteByte(table->m_nChangeCount);
	response.WriteInt(table->m_nChangeCoin1);
	response.WriteInt(table->m_nChangeCoin2);
	response.WriteInt(table->m_nChangeCoin3);
	response.End();
	if(HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false) < 0)
		_LOG_ERROR_("[GetTableDetailProc] Send To Uid[%d] Error!\n", player->id);

	return 0;
}


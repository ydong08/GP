#include <string>
#include "LoginComingProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "MySqlConnect.h"
#include "MoneyConnect.h"
#include "RoundServerConnect.h"
#include "UdpManager.h"
#include "LogServerConnect.h"

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
	uint64_t money = pPacket->ReadInt64();
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
	_LOG_DEBUG_("[DATA Parse] money=[%lu] \n", money);
	_LOG_DEBUG_("[DATA Parse] level=[%d] \n", level);
	_LOG_DEBUG_("[DATA Parse] json=[%s] \n", json.c_str());
	_LOG_DEBUG_("[DATA Parse] userip=[%s] \n", userip.c_str());

	ClientHandler* hallhandler = reinterpret_cast<ClientHandler*> (clientHandler);

	MoneyNodes* connect = MoneyConnectManager::getNodes(uid % 10 + 1);
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
	
	if (connect->send(&response,false) > 0)
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
		return sendErrorMsg(hallhandler, cmd, uid, -27, ErrorMsg::getInstance()->getErrMsg(-27), seq);
	}

	player->id = uid;
	player->money = money;
	player->hallid = hallhandler->hallid;
	strncpy(player->name, name.c_str(), sizeof(player->name) - 1);
	player->name[sizeof(player->name) - 1] = '\0';
	strcpy(player->json, json.c_str());
	player->status = STATUS_PLAYER_COMING;
	player->source = source;
	Table* table = NULL;
	table = room->getTable(realTid);
	player->login();

	if(!player->checkMoney())
	{
		_LOG_ERROR_("UID[%d] money is too small money[%d]\n", uid, player->money);
		return sendErrorMsg(hallhandler, cmd, uid, -4, ErrorMsg::getInstance()->getErrMsg(-4), seq);
	}

	if(table == NULL)
	{
		_LOG_WARN_("[This Table[%d] is Full] \n", realTid );
		table = room->getAvailableTable();
		if(table == NULL)
		{
			_LOG_ERROR_("[Room is full, no table for player]\n", realTid );
			return sendErrorMsg(hallhandler, cmd, uid, -21, ErrorMsg::getInstance()->getErrMsg(-21), seq);
		}
	}

	_LOG_INFO_("[LoginComingProc] UID=[%d] NorLogin OK ,seat_id[%d]\n", player->id,player->seat_id );

	if(table->playerComming(player) != 0)
	{
		_LOG_WARN_("Player[%d] Coming This table[%d] Error\n", player->id, table->id);
		//如果用户进入桌子失败，则重新给用户找一个桌子进入
		table = room->getAvailableTable();
		if(table!= NULL)
		{
			_LOG_WARN_("Trans Table[%d] Coming Error, Player[%d] Coming Other table[%d]\n", tid, player->id, table->id);
			if(table->playerComming(player) != 0)
				return sendErrorMsg(hallhandler, cmd, uid, -28, ErrorMsg::getInstance()->getErrMsg(-28), seq);
		}
		else
			return sendErrorMsg(hallhandler, cmd, uid, -21, ErrorMsg::getInstance()->getErrMsg(-21), seq);
	}
	player->setActiveTime(time(NULL));
	int i= 0;
	int sendnum = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->countPlayer)
			break;
		if(table->player_array[i])
		{
			sendTabePlayersInfo(table->player_array[i], table, table->countPlayer, player->id, seq);
			sendnum++;
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
	response.WriteShort(player->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteInt(comuid);
	response.WriteShort(player->seat_id);
	response.WriteShort(player->tab_index);
	response.WriteShort(num);
	_LOG_DEBUG_("<==[LoginComingProc] Push [0x%04x]\n", CLIENT_MSG_LOGINCOMING);
	_LOG_DEBUG_("[Data Response] push to uid=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] push to status=[%d]\n", player->status);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] seat_id=[%d]\n", player->seat_id);
	_LOG_DEBUG_("[Data Response] comuid=[%d]\n", comuid);
	_LOG_DEBUG_("[Data Response] num=[%d]\n", num);
	int sendnum = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == num)
			break;
		Player* otherplayer = table->player_array[i];
		if(otherplayer)
		{
			response.WriteInt(otherplayer->id);
			response.WriteString(otherplayer->name);
			response.WriteShort(otherplayer->status);
			response.WriteShort(otherplayer->tab_index);
			response.WriteInt64(otherplayer->money);
			response.WriteInt64(otherplayer->carrycoin);
			response.WriteString(otherplayer->json);

			/*_LOG_DEBUG_("[Data Response] uid=[%d]\n", otherplayer->id);
			_LOG_DEBUG_("[Data Response] name=[%s]\n", otherplayer->name);
			_LOG_DEBUG_("[Data Response] ustatus=[%d]\n", otherplayer->status);
			_LOG_DEBUG_("[Data Response] tab_index=[%d]\n", otherplayer->tab_index);
			_LOG_DEBUG_("[Data Response] money=[%lu]\n", otherplayer->money);
			_LOG_DEBUG_("[Data Response] carrycoin=[%lu]\n", otherplayer->carrycoin);
			_LOG_DEBUG_("[Data Response] json=[%s]\n", otherplayer->json);*/
			sendnum++;
		}
	}
	if(player->istask && !player->isroundtask)
	{
		response.WriteInt(player->voucher);
		_LOG_DEBUG_("[Data Response] voucher=[%d]\n", player->voucher);
		int ntask = player->boardTask;
		int nlow = ntask & 0x00FF;
		int nmid = ntask>>8 & 0x00FF;
		int nhigh = ntask>>16 & 0x00FF;
		int ncomplete = ntask >> 24;
		if(Configure::getInstance()->level == 1)
		{
			if(ncomplete & 1)
			{
				if(ncomplete & 2)
				{
					if(ncomplete & 4)
					{
						response.WriteShort(0);
						response.WriteString("");
						response.WriteShort(0);
						response.WriteShort(0);
						response.WriteShort(0);
					}
					else
					{
						response.WriteShort(1);
						response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
						response.WriteShort(nhigh);
						response.WriteShort(player->coincfg.playCount3);
						response.WriteShort(player->coincfg.getingot3);
					}
				}
				else
				{
					response.WriteShort(1);
					response.WriteString(ErrorMsg::getInstance()->getErrMsg(5));
					response.WriteShort(nmid);
					response.WriteShort(player->coincfg.playCount2);
					response.WriteShort(player->coincfg.getingot2);
				}
			}
			else
			{
				response.WriteShort(1);
				response.WriteString(ErrorMsg::getInstance()->getErrMsg(4));
				response.WriteShort(nlow);
				response.WriteShort(player->coincfg.playCount1);
				response.WriteShort(player->coincfg.getingot1);
				
			}
		}

		if(Configure::getInstance()->level == 2)
		{
			if(ncomplete & 2)
			{
				if(ncomplete & 4)
				{
					response.WriteShort(0);
					response.WriteString("");
					response.WriteShort(0);
					response.WriteShort(0);
					response.WriteShort(0);
				}
				else
				{
					response.WriteShort(1);
					response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
					response.WriteShort(nhigh);
					response.WriteShort(player->coincfg.playCount3);
					response.WriteShort(player->coincfg.getingot3);
				}
			}
			else
			{
				response.WriteShort(1);
				response.WriteString(ErrorMsg::getInstance()->getErrMsg(5));
				response.WriteShort(nmid);
				response.WriteShort(player->coincfg.playCount2);
				response.WriteShort(player->coincfg.getingot2);
			}
		}

		if(Configure::getInstance()->level == 3)
		{
			if(ncomplete & 4)
			{
				response.WriteShort(0);
				response.WriteString("");
				response.WriteShort(0);
				response.WriteShort(0);
				response.WriteShort(0);
			}
			else
			{
				response.WriteShort(1);
				response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
				response.WriteShort(nhigh);
				response.WriteShort(player->coincfg.playCount3);
				response.WriteShort(player->coincfg.getingot3);
			}
		}

		if(Configure::getInstance()->level > 3)
		{
			response.WriteShort(0);
			response.WriteString("");
			response.WriteShort(0);
			response.WriteShort(0);
			response.WriteShort(0);
		}
	}
	if(player->isroundtask)
	{
		response.WriteInt(player->voucher);
		response.WriteShort(0);
		response.WriteString("");
		response.WriteShort(0);
		response.WriteShort(0);
		response.WriteShort(0);
		int comflag = 0;
		int roundnum = 0;
		if(Configure::getInstance()->level == 1)
		{
			comflag = player->m_nRoundComFlag & 0x0F;
			roundnum = player->m_nRoundNum & 0x00FF;
		}
		if(Configure::getInstance()->level == 2)
		{
			comflag = (player->m_nRoundComFlag >> 4) & 0x0F;
			roundnum = (player->m_nRoundNum >> 8) & 0x00FF;
		}
		if(Configure::getInstance()->level == 3)
		{
			comflag = (player->m_nRoundComFlag >> 8) & 0x0F;
			roundnum = (player->m_nRoundNum >> 16) & 0x00FF;
		}

		_LOG_DEBUG_("comflag:%d roundnum:%d\n", comflag, roundnum);

		if(table->m_nRoundNum3 != 0)
		{
			_LOG_DEBUG_("m_nRoundNum3:%d m_nRoundNum2:%d m_nRoundNum1:%d\n", table->m_nRoundNum3, table->m_nRoundNum2, table->m_nRoundNum1);
			//都完成了
			if(comflag & 4)
			{
				response.WriteByte(0);
				response.WriteByte(0);
			}
			//完成中间的
			else if (comflag & 2)
			{
				response.WriteByte(roundnum - (table->m_nRoundNum1 + table->m_nRoundNum2));
				response.WriteByte(table->m_nRoundNum3);
				if(table->m_bRewardType == 2)
				{
					if(player->m_nComGetRoll == 0)
						player->m_nComGetRoll = table->m_nGetRollLow3 + rand()%((table->m_nGetRollHigh3 - table->m_nGetRollLow3) <= 0 ? 1 : (table->m_nGetRollHigh3 - table->m_nGetRollLow3+1));
				}
				else
				{
					if(player->m_nComGetCoin == 0)
						player->m_nComGetCoin = table->m_nGetCoinLow3 + rand()%((table->m_nGetCoinHigh3 - table->m_nGetCoinLow3) <= 0 ? 1 : (table->m_nGetCoinHigh3 - table->m_nGetCoinLow3+1));
				}
			}
			//完成第一个局数
			else if (comflag & 1)
			{
				response.WriteByte(roundnum - table->m_nRoundNum1);
				response.WriteByte(table->m_nRoundNum2);
				if(table->m_bRewardType == 2)
				{
					if(player->m_nComGetRoll == 0)
						player->m_nComGetRoll = table->m_nGetRollLow2 + rand()%((table->m_nGetRollHigh2 - table->m_nGetRollLow2) <= 0 ? 1 : (table->m_nGetRollHigh2 - table->m_nGetRollLow2+1));
				}
				else
				{
					if(player->m_nComGetCoin == 0)
						player->m_nComGetCoin = table->m_nGetCoinLow2 + rand()%((table->m_nGetCoinHigh2 - table->m_nGetCoinLow2) <= 0 ? 1 : (table->m_nGetCoinHigh2 - table->m_nGetCoinLow2+1));
				}
			}
			//所有的都没有完成
			else
			{
				response.WriteByte(roundnum);
				response.WriteByte(table->m_nRoundNum1);
				if(table->m_bRewardType == 2)
				{
					if(player->m_nComGetRoll == 0)
						player->m_nComGetRoll = table->m_nGetRollLow1 + rand()%((table->m_nGetRollHigh1 - table->m_nGetRollLow1) <= 0 ? 1 : (table->m_nGetRollHigh1 - table->m_nGetRollLow1+1));
				}
				else
				{
					if(player->m_nComGetCoin == 0)
						player->m_nComGetCoin = table->m_nGetCoinLow1 + rand()%((table->m_nGetCoinHigh1 - table->m_nGetCoinLow1) <= 0 ? 1 : (table->m_nGetCoinHigh1 - table->m_nGetCoinLow1+1));
				}
			}
		}
		else if(table->m_nRoundNum2 != 0)
		{
			//完成中间的
			if (comflag & 2)
			{
				response.WriteByte(0);
				response.WriteByte(0);
			}
			//完成第一个局数
			else if (comflag & 1)
			{
				response.WriteByte(roundnum - table->m_nRoundNum1);
				response.WriteByte(table->m_nRoundNum2);
				if(table->m_bRewardType == 2)
				{
					if(player->m_nComGetRoll == 0)
						player->m_nComGetRoll = table->m_nGetRollLow2 + rand()%((table->m_nGetRollHigh2 - table->m_nGetRollLow2) <= 0 ? 1 : (table->m_nGetRollHigh2 - table->m_nGetRollLow2+1));
				}
				else
				{
					if(player->m_nComGetCoin == 0)
						player->m_nComGetCoin = table->m_nGetCoinLow2 + rand()%((table->m_nGetCoinHigh2 - table->m_nGetCoinLow2) <= 0 ? 1 : (table->m_nGetCoinHigh2 - table->m_nGetCoinLow2+1));
				}
			}
			//所有的都没有完成
			else
			{
				response.WriteByte(roundnum);
				response.WriteByte(table->m_nRoundNum1);
				if(table->m_bRewardType == 2)
				{
					if(player->m_nComGetRoll == 0)
						player->m_nComGetRoll = table->m_nGetRollLow1 + rand()%((table->m_nGetRollHigh1 - table->m_nGetRollLow1) <= 0 ? 1 : (table->m_nGetRollHigh1 - table->m_nGetRollLow1+1));
				}
				else
				{
					if(player->m_nComGetCoin == 0)
						player->m_nComGetCoin = table->m_nGetCoinLow1 + rand()%((table->m_nGetCoinHigh1 - table->m_nGetCoinLow1) <= 0 ? 1 : (table->m_nGetCoinHigh1 - table->m_nGetCoinLow1+1));
				}
			}
		}
		else if (table->m_nRoundNum1 != 0)
		{
			//完成第一个局数
			if (comflag & 1)
			{
				response.WriteByte(0);
				response.WriteByte(0);
			}
			//所有的都没有完成
			else
			{
				response.WriteByte(roundnum);
				response.WriteByte(table->m_nRoundNum1);
				if(table->m_bRewardType == 2)
				{
					if(player->m_nComGetRoll == 0)
						player->m_nComGetRoll = table->m_nGetRollLow1 + rand()%((table->m_nGetRollHigh1 - table->m_nGetRollLow1) <= 0 ? 1 : (table->m_nGetRollHigh1 - table->m_nGetRollLow1+1));
				}
				else
				{
					if(player->m_nComGetCoin == 0)
						player->m_nComGetCoin = table->m_nGetCoinLow1 + rand()%((table->m_nGetCoinHigh1 - table->m_nGetCoinLow1) <= 0 ? 1 : (table->m_nGetCoinHigh1 - table->m_nGetCoinLow1+1));
				}
			}
		}
		else
		{
			response.WriteByte(0);
			response.WriteByte(0);
		}
		response.WriteByte(table->m_bRewardType);
		if(table->m_bRewardType == 2)
		{
			response.WriteInt(player->m_nComGetRoll);
			response.WriteInt(Configure::getInstance()->rewardroll);
		}
		else
		{
			response.WriteInt(player->m_nComGetCoin);
			response.WriteInt(Configure::getInstance()->rewardcoin);
		}
	}
	response.End();
	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
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

	ClientHandler* hallhandler = reinterpret_cast<ClientHandler*> (pt->client);

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
	Player* player = room->getPlayerUID(uid);
	if(player)
	{
		Table* table = room->getTable(player->tid);
		if(table)
		{
			sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),pt->seq);
			_LOG_WARN_("player[%d] has in this table[%d] can't coming other table[%d]\n", player->id, player->tid, realTid);
			return 0;
		}
	}
	
	if (player == NULL)
		player = room->getAvailablePlayer();
	if(player == NULL)
	{
		_LOG_ERROR_("[Room is full,no seat for player] \n" );
		return sendErrorMsg(hallhandler, cmd, uid, -27, ErrorMsg::getInstance()->getErrMsg(-27), pt->seq);
	}
	
	player->id = uid;
	player->money = money;
	player->hallid = hallhandler->hallid;
	strncpy(player->name, name.c_str(), sizeof(player->name) - 1);
	player->name[sizeof(player->name) - 1] = '\0';
	strcpy(player->json, json.c_str());
	player->status = STATUS_PLAYER_COMING;
	player->source = source;
	//player->nwin = nwin;
	//player->nlose = nlose;
	player->voucher = roll + roll1;
	strcpy(player->m_byIP, userip.c_str());

	player->login();
	if(!player->checkMoney())
	{
		_LOG_ERROR_("UID[%d] money is too small money[%d]\n", uid, player->money);
		sendErrorMsg(hallhandler, cmd, uid, -4,ErrorMsg::getInstance()->getErrMsg(-4),pt->seq);
		player->setEnterTime(time(NULL));
		player->leave();
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
			return sendErrorMsg(hallhandler, cmd, uid, -21,ErrorMsg::getInstance()->getErrMsg(-21),pt->seq);
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
				return sendErrorMsg(hallhandler, cmd, uid, -21,ErrorMsg::getInstance()->getErrMsg(-21),pt->seq);
			}
		}
	}
	
	bool iscan = true;
	//相同IP不能进入同一桌
	if(Configure::getInstance()->level >= Configure::getInstance()->contrllevel &&
		Configure::getInstance()->level != 5)
	{
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(table->player_array[i] && table->player_array[i]->source != 30)
			{
				if(!iscan)
					break;
				if(strlen(player->m_byIP) < 2 || strlen(table->player_array[i]->m_byIP) < 2)
					break;

				if(strcmp(table->player_array[i]->m_byIP, player->m_byIP) == 0)
				{
					iscan = false;
					break;
				}
			}
		}

		if(!iscan)
		{
			table = room->getAvailableTable(table->id);
			if(table == NULL)
			{
				_LOG_ERROR_("[Room is full,!iscan no table[%d] for player]\n", realTid );
				return sendErrorMsg(hallhandler, cmd, uid, -21,ErrorMsg::getInstance()->getErrMsg(-21),pt->seq);
			}

			_LOG_WARN_("Trans Table[%d] !iscan Coming Error, Player[%d] Coming Other table[%d]\n", tid, player->id, table->id);
		}
	}

	player->setRePlayeTime(time(NULL));
	_LOG_INFO_("[LoginComingProc] UID=[%d] NorLogin OK ,seat_id[%d]\n", player->id,player->seat_id );

	if(player->istask && !player->isroundtask)
	{
		player->boardTask = TaskManager::getInstance()->getNewTask(player);
		if(player->boardTask < 0)
			player->boardTask = 0;
		_LOG_DEBUG_("player[%d] boardTask[%d]\n", player->id, player->boardTask);
	}

	if(table->playerComming(player) != 0)
	{
		_LOG_WARN_("Player[%d] Coming This table[%d] Error\n", player->id, table->id);
		//如果用户进入桌子失败，则重新给用户找一个桌子进入
		table = room->getAvailableTable();
		if(table!= NULL)
		{
			_LOG_WARN_("Trans Table[%d] Coming Error, Player[%d] Coming Other table[%d]\n", tid, player->id, table->id);
			if(table->playerComming(player) != 0)
				return sendErrorMsg(hallhandler, cmd, uid, -28,ErrorMsg::getInstance()->getErrMsg(-28),pt->seq);
		}
		else
			return sendErrorMsg(hallhandler, cmd, uid, -21,ErrorMsg::getInstance()->getErrMsg(-21),pt->seq);
	}
	
// 	if(table == NULL)
// 		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),pt->seq);

	OutputPacket response;
	response.Begin(ROUND_GET_INFO, player->id);
	response.WriteInt(player->id);
	response.WriteShort(table->id);
	response.End();
	if(player->source != 30)
	{
		if(RoundServerConnect::getInstance()->Send(&response) < 0)
			_LOG_ERROR_("[RoundServerConnect send Error uid[%d]]\n", player->id);
	}

	Json::Value data;
	data["errcode"] = 0;
	_UDP_REPORT_(player->id, cmd,"%s",data.toStyledString().c_str());
	

	player->setActiveTime(time(NULL));
	int i= 0;
	int sendnum = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->countPlayer)
			break;
		if(table->player_array[i])
		{
			sendTabePlayersInfo(table->player_array[i], table, table->countPlayer, player->id, pt->seq);
			sendnum++;
		}
	}

	if(table->isActive())
	{
		sendTableInfo(player, table);
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
	int svid = Configure::getInstance()->server_id;
	int tid = svid << 16|table->id;
	Player* betplayer = table->getPlayer(table->betUid);
	short remaintime = 0;
	//当前超时时间
	if(betplayer)
		remaintime = Configure::getInstance()->betcointime - (time(NULL)-betplayer->GetBetCoinTime());
	OutputPacket response;
	response.Begin(CLIENT_MSG_TABLEDET, player->id);
	response.WriteShort(0);
	response.WriteString("ok");
	response.WriteInt(player->id);
	response.WriteShort(player->status);
	response.WriteInt(tid);
	response.WriteShort(table->status);
	response.WriteByte(player->hascard ? player->card_array[0] : 0);
	response.WriteShort(table->currRound);
	response.WriteInt64(table->ante);
	response.WriteInt64(table->thisGameLimit);
	short currRound = table->currRound;
	response.WriteInt(table->betUid);
	response.WriteShort(betplayer ? betplayer->optype : 0);	
	response.WriteShort(remaintime);
	response.WriteShort(Configure::getInstance()->betcointime - 3);
	response.WriteShort(table->leavercount);
	for(int j = 0; j < table->leavercount; ++j)
	{
		response.WriteInt(table->leaverarry[j].uid);
		response.WriteShort(table->leaverarry[j].tab_index);
		response.WriteString(table->leaverarry[j].name);
		response.WriteInt64(table->leaverarry[j].betcoin);
	}
	response.WriteShort(table->countPlayer);

	_LOG_DEBUG_("<==[sendTableInfo] Push [0x%04x]\n", CLIENT_MSG_TABLEDET);
	_LOG_DEBUG_("[Data Response] push to uid=[%d]\n", player->id);
	_LOG_DEBUG_("[Data Response] push to status=[%d]\n", player->status);
	_LOG_DEBUG_("[Data Response] tid=[%d]\n", tid);
	_LOG_DEBUG_("[Data Response] tstatus=[%d]\n", table->status);
	_LOG_DEBUG_("[Data Response] currRound=[%d]\n", table->currRound);
	_LOG_DEBUG_("[Data Response] betUid=[%d]\n", table->betUid);
	_LOG_DEBUG_("[Data Response] betoptype=[%d]\n", betplayer ? betplayer->optype : 0);
	_LOG_DEBUG_("[Data Response] remaintime=[%d]\n", remaintime);
	_LOG_DEBUG_("[Data Response] betcointime=[%d]\n", Configure::getInstance()->betcointime - 3);
	_LOG_DEBUG_("[Data Response] leavercount=[%d]\n", table->leavercount);
	_LOG_DEBUG_("[Data Response] countPlayer=[%d]\n", table->countPlayer);
	int i= 0;
	int sendnum = 0;
	for(i = 0; i < GAME_PLAYER; ++i)
	{
		if(sendnum == table->countPlayer)
			break;
		Player* getPlayer = table->player_array[i];
		if(getPlayer)
		{
			response.WriteInt(getPlayer->id);
			response.WriteString(getPlayer->name);
			response.WriteShort(getPlayer->status);
			response.WriteShort(getPlayer->tab_index);
			response.WriteInt64(getPlayer->money);
			response.WriteInt64(getPlayer->carrycoin);
			response.WriteInt64(getPlayer->betCoinList[0]);
			response.WriteInt64(getPlayer->betCoinList[table->currRound]);
			response.WriteInt64(table->currMaxCoin - getPlayer->betCoinList[table->currRound]);
			response.WriteInt64(getPlayer->nextlimitcoin);
			response.WriteInt(getPlayer->nwin);
			response.WriteInt(getPlayer->nlose);
			response.WriteString(getPlayer->json);
			_LOG_DEBUG_("[Data Response] getPlayerid=[%d]\n", getPlayer->id);
			_LOG_DEBUG_("[Data Response] getPlayer->status=[%d]\n", getPlayer->status);
			_LOG_DEBUG_("[Data Response] getPlayer->tab_index=[%d]\n", getPlayer->tab_index);
			_LOG_DEBUG_("[Data Response] getPlayer->money=[%ld]\n", getPlayer->money);
			_LOG_DEBUG_("[Data Response] getPlayer->carrycoin=[%ld]\n", getPlayer->carrycoin);
			_LOG_DEBUG_("[Data Response] getPlayer->betCoinList[0]=[%ld]\n", getPlayer->betCoinList[0]);
			_LOG_DEBUG_("[Data Response] getPlayer->betCoinList[table->currRound]=[%ld]\n", getPlayer->betCoinList[table->currRound]);
			_LOG_DEBUG_("[Data Response] callmoney=[%ld]\n", table->currMaxCoin - getPlayer->betCoinList[table->currRound]);
			_LOG_DEBUG_("[Data Response] nextlimitcoin=[%ld]\n",getPlayer->nextlimitcoin);
			_LOG_DEBUG_("[Data Response] nwin=[%ld]\n",getPlayer->nwin);
			_LOG_DEBUG_("[Data Response] nlose=[%ld]\n",getPlayer->nlose);
			if(getPlayer->hascard)
			{
				response.WriteShort(1);
				//当前还没有到所有要亮底牌的时候
				if(currRound != 6)
				{
					//如果第五轮则发牌也只是发明牌的
					if(currRound == 5)
					{
						for(int j = 1; j < 5; j++)
							response.WriteByte(getPlayer->card_array[j]);
					}
					else
					{
						for(int j = 1; j < currRound + 1; j++)
							response.WriteByte(getPlayer->card_array[j]);
					}
					//返回当前用户手牌类型
					short cardType = table->GetPlayerCardKind(getPlayer->card_array, 1, currRound == 5 ? 4 : currRound);
					response.WriteShort(cardType); 
				}
				else
				{
					//如果是第六轮直接把所有玩家的底牌都发过去
					for(int j = 0; j < 5; j++)
						response.WriteByte(getPlayer->card_array[j]);
					//返回当前用户手牌类型
					short cardType = table->GetPlayerCardKind(getPlayer->card_array, 0, 4);
					response.WriteShort(cardType); 
				}
			}
			else
				response.WriteShort(0);

			sendnum++;
		}
	}

	if(player->task1)
	{
		response.WriteInt64(player->task1->taskid);
		response.WriteString(player->source == 1 ? player->task1->taskANDname : player->task1->taskIOSname);
		response.WriteShort(player->ningot);
		response.WriteInt(player->voucher);
	}
	else
	{
		response.WriteInt64(0);
		response.WriteString("");
		response.WriteShort(0);
		response.WriteInt(0);
	}

	if(player->istask && !player->isroundtask)
	{
		int ntask = player->boardTask;
		int nlow = ntask & 0x00FF;
		int nmid = ntask>>8 & 0x00FF;
		int nhigh = ntask>>16 & 0x00FF;
		int ncomplete = ntask >> 24;
		if(Configure::getInstance()->level == 1)
		{
			if(ncomplete & 1)
			{
				if(ncomplete & 2)
				{
					if(ncomplete & 4)
					{
						response.WriteShort(0);
						response.WriteString("");
						response.WriteShort(0);
						response.WriteShort(0);
						response.WriteShort(0);
						_LOG_DEBUG_("isshow 0\n");
					}
					else
					{
						response.WriteShort(1);
						response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
						response.WriteShort(nhigh);
						response.WriteShort(player->coincfg.playCount3);
						response.WriteShort(player->coincfg.getingot3);
						_LOG_DEBUG_("isshow 1 des:[%s] nhigh:[%d] playcount[%d] getingot[%d]\n", 
						ErrorMsg::getInstance()->getErrMsg(6),nhigh,player->coincfg.playCount3,player->coincfg.getingot3);
					}
				}
				else
				{
					response.WriteShort(1);
					response.WriteString(ErrorMsg::getInstance()->getErrMsg(5));
					response.WriteShort(nmid);
					response.WriteShort(player->coincfg.playCount2);
					response.WriteShort(player->coincfg.getingot2);
					_LOG_DEBUG_("isshow 1 des:[%s] nmid:[%d] playcount[%d] getingot[%d]\n", 
						ErrorMsg::getInstance()->getErrMsg(5),nmid,player->coincfg.playCount2,player->coincfg.getingot2);
				}
			}
			else
			{
				response.WriteShort(1);
				_LOG_DEBUG_("isshow 1\n");
				response.WriteString(ErrorMsg::getInstance()->getErrMsg(4));
				response.WriteShort(nlow);
				response.WriteShort(player->coincfg.playCount1);
				response.WriteShort(player->coincfg.getingot1);
				_LOG_DEBUG_("isshow 1 des:[%s] nlow:[%d] playcount[%d] getingot[%d]\n", 
						ErrorMsg::getInstance()->getErrMsg(4),nlow,player->coincfg.playCount1,player->coincfg.getingot1);
			}
		}

		if(Configure::getInstance()->level == 2)
		{
			if(ncomplete & 2)
			{
				if(ncomplete & 4)
				{
					response.WriteShort(0);
					response.WriteString("");
					response.WriteShort(0);
					response.WriteShort(0);
					response.WriteShort(0);
				}
				else
				{
					response.WriteShort(1);
					response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
					response.WriteShort(nhigh);
					response.WriteShort(player->coincfg.playCount3);
					response.WriteShort(player->coincfg.getingot3);
					_LOG_DEBUG_("isshow 1 des:[%s] nhigh:[%d] playcount[%d] getingot[%d]\n", 
						ErrorMsg::getInstance()->getErrMsg(6),nhigh,player->coincfg.playCount3,player->coincfg.getingot3);
				}
			}
			else
			{
				response.WriteShort(1);
				response.WriteString(ErrorMsg::getInstance()->getErrMsg(5));
				response.WriteShort(nmid);
				response.WriteShort(player->coincfg.playCount2);
				response.WriteShort(player->coincfg.getingot2);
				_LOG_DEBUG_("isshow 1 des:[%s] nmid:[%d] playcount[%d] getingot[%d]\n", 
						ErrorMsg::getInstance()->getErrMsg(5),nmid,player->coincfg.playCount2,player->coincfg.getingot2);
			}
		}

		if(Configure::getInstance()->level == 3)
		{
			if(ncomplete & 4)
			{
				response.WriteShort(0);
				response.WriteString("");
				response.WriteShort(0);
				response.WriteShort(0);
				response.WriteShort(0);
			}
			else
			{
				response.WriteShort(1);
				response.WriteString(ErrorMsg::getInstance()->getErrMsg(6));
				response.WriteShort(nhigh);
				response.WriteShort(player->coincfg.playCount3);
				response.WriteShort(player->coincfg.getingot3);
				_LOG_DEBUG_("isshow 1 des:[%s] nhigh:[%d] playcount[%d] getingot[%d]\n", 
						ErrorMsg::getInstance()->getErrMsg(6),nhigh,player->coincfg.playCount3,player->coincfg.getingot3);
			}
		}
	}
	if(player->isroundtask)
	{
		response.WriteShort(0);
		response.WriteString("");
		response.WriteShort(0);
		response.WriteShort(0);
		response.WriteShort(0);
		int comflag = 0;
		int roundnum = 0;
		if(Configure::getInstance()->level == 1)
		{
			comflag = player->m_nRoundComFlag & 0x0F;
			roundnum = player->m_nRoundNum & 0x00FF;
		}
		if(Configure::getInstance()->level == 2)
		{
			comflag = (player->m_nRoundComFlag >> 4) & 0x0F;
			roundnum = (player->m_nRoundNum >> 8) & 0x00FF;
		}
		if(Configure::getInstance()->level == 3)
		{
			comflag = (player->m_nRoundComFlag >> 8) & 0x0F;
			roundnum = (player->m_nRoundNum >> 16) & 0x00FF;
		}

		_LOG_DEBUG_("comflag:%d roundnum:%d\n", comflag, roundnum);

		if(table->m_nRoundNum3 != 0)
		{
			_LOG_DEBUG_("m_nRoundNum3:%d m_nRoundNum2:%d m_nRoundNum1:%d\n", table->m_nRoundNum3, table->m_nRoundNum2, table->m_nRoundNum1);
			//都完成了
			if(comflag & 4)
			{
				response.WriteByte(0);
				response.WriteByte(0);
			}
			//完成中间的
			else if (comflag & 2)
			{
				response.WriteByte(roundnum - (table->m_nRoundNum1 + table->m_nRoundNum2));
				response.WriteByte(table->m_nRoundNum3);
			}
			//完成第一个局数
			else if (comflag & 1)
			{
				response.WriteByte(roundnum - table->m_nRoundNum1);
				response.WriteByte(table->m_nRoundNum2);
			}
			//所有的都没有完成
			else
			{
				response.WriteByte(roundnum);
				response.WriteByte(table->m_nRoundNum1);
			}
		}
		else if(table->m_nRoundNum2 != 0)
		{
			//完成中间的
			if (comflag & 2)
			{
				response.WriteByte(0);
				response.WriteByte(0);
			}
			//完成第一个局数
			else if (comflag & 1)
			{
				response.WriteByte(roundnum - table->m_nRoundNum1);
				response.WriteByte(table->m_nRoundNum2);
			}
			//所有的都没有完成
			else
			{
				response.WriteByte(roundnum);
				response.WriteByte(table->m_nRoundNum1);
			}
		}
		else if (table->m_nRoundNum1 != 0)
		{
			//完成第一个局数
			if (comflag & 1)
			{
				response.WriteByte(0);
				response.WriteByte(0);
			}
			//所有的都没有完成
			else
			{
				response.WriteByte(roundnum);
				response.WriteByte(table->m_nRoundNum1);
			}
		}
		else
		{
			response.WriteByte(0);
			response.WriteByte(0);
		}
		response.WriteByte(table->m_bRewardType);
		if(table->m_bRewardType == 2)
		{
			response.WriteInt(player->m_nComGetRoll);
			response.WriteInt(Configure::getInstance()->rewardroll);
		}
		else
		{
			response.WriteInt(player->m_nComGetCoin);
			response.WriteInt(Configure::getInstance()->rewardcoin);
		}
	}
	response.End();
	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
		_LOG_ERROR_("[GetTableDetailProc] Send To Uid[%d] Error!\n", player->id);

	return 0;

}


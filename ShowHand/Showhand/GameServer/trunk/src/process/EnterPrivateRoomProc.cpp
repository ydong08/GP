#include <string>
#include "EnterPrivateRoomProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "MySqlConnect.h"
#include "MoneyConnect.h"
#include "RoundServerConnect.h"
#include "TaskRedis.h"
#include "UdpManager.h"
#include "LogServerConnect.h"

using namespace std;

struct Param
{
	int uid;
	int tid;
	short source;
	char name[64];
	char password[64];
	char json[1024];
};

EnterPrivateRoomProc::EnterPrivateRoomProc()
{
	this->name = "EnterPrivateRoomProc" ;
}

EnterPrivateRoomProc::~EnterPrivateRoomProc()
{
}

int EnterPrivateRoomProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	int uid = pPacket->ReadInt();
	string name = pPacket->ReadString();
	int tid = pPacket->ReadInt();
	uint64_t money = pPacket->ReadInt64();
	short level = pPacket->ReadShort();
	string password = pPacket->ReadString();
	string json = pPacket->ReadString();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	
	_LOG_INFO_("==>[EnterPrivateRoomProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d]\n", tid, svid, realTid);
	_LOG_DEBUG_("[DATA Parse] name=[%s] \n", name.c_str());
	_LOG_DEBUG_("[DATA Parse] money=[%lu] \n", money);
	_LOG_DEBUG_("[DATA Parse] level=[%d] \n", level);
	_LOG_DEBUG_("[DATA Parse] json=[%s] \n", json.c_str());

	ClientHandler* hallhandler = reinterpret_cast<ClientHandler*> (clientHandler);

	if(level != Configure::getInstance()->level)
	{
		_LOG_ERROR_("level[%d] is Not equal configure level [%d]\n", level,  Configure::getInstance()->level);
		return sendErrorMsg(hallhandler, cmd, uid, -3, ErrorMsg::getInstance()->getErrMsg(-3), seq);
	}

	if(TaskRedis::getInstance()->isUserInGame(uid))
	{
		_LOG_ERROR_("level[%d] You Have in Other Game [%d]\n", level,  Configure::getInstance()->level);
		return sendErrorMsg(hallhandler, cmd, uid, -25, ErrorMsg::getInstance()->getErrMsg(-25), seq);
	}


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


	if (connect->send(&response, false) > 0)
	{
		_LOG_DEBUG_("Transfer request to Back_MYSQLServer OK\n");
		struct Param* param = (struct Param *) malloc (sizeof(struct Param));;
		param->uid = uid;
		param->tid = tid;
		param->source = source;
		strncpy(param->name, name.c_str(),63);	
		param->name[63] = '\0';
		strncpy(param->password, password.c_str(), 63);
		param->password[63] = '\0';
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

	Player* player = room->getAvailablePlayer();
	if(player==NULL)
	{
		_LOG_ERROR_("[Room is full,no seat for player] \n" );
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),seq);
	}
	

	player->id = uid;
	player->money = money;
	player->hallid = hallhandler->hallid;
	strncpy(player->name, name.c_str(), sizeof(player->name) - 1);
	player->name[sizeof(player->name) - 1] = '\0';
	strcpy(player->json, json.c_str());
	player->status = STATUS_PLAYER_COMING;
	player->source = source;
	player->login();
	Table* table = NULL;
	table = room->getTable(realTid);

	if(!player->checkMoney())
	{
		_LOG_ERROR_("UID[%d] money is too small money[%d]\n", uid, player->money);
		sendErrorMsg(hallhandler, cmd, uid, -4,ErrorMsg::getInstance()->getErrMsg(-4),seq);
		player->leave();
		return 0;
	}

	if(table == NULL)
	{
		_LOG_ERROR_("[Room is full, no table for player uid[%d] realtid[%d]]\n", uid, realTid );
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
	}

	_LOG_INFO_("[EnterPrivateRoomProc] UID=[%d] NorLogin OK ,seat_id[%d]\n", player->id,player->seat_id );
	
	if(table->haspasswd && strcmp(table->getPassword(), password.c_str()) != 0)
	{
		_LOG_ERROR_("uid[%d] tid[%d] password is not correct\n", uid, realTid );
		return sendErrorMsg(hallhandler, cmd, uid, -5,ErrorMsg::getInstance()->getErrMsg(-5),seq);
	}

	if(table->playerComming(player) != 0)
	{
		_LOG_WARN_("Trans Table[%d] realTid[%d] Coming Error, Player[%d]\n", tid, realTid, player->id);
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),seq);
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

int EnterPrivateRoomProc::sendTabePlayersInfo(Player* player, Table* table, short num, int comuid, short seq)
{
	int svid = Configure::getInstance()->server_id;
	int tid = svid << 16|table->id;
	OutputPacket response;
	if(player->id == comuid)
	{
		response.Begin(CLIENT_ENTER_PRIVATE, player->id);
		_LOG_DEBUG_("<==[EnterPrivateRoomProc] Push [0x%04x]\n", CLIENT_ENTER_PRIVATE);
		response.SetSeqNum(seq);
	}
	else
	{
		response.Begin(CLIENT_MSG_LOGINCOMING, player->id);
		_LOG_DEBUG_("<==[LoginComingProc] Push [0x%04x]\n", CLIENT_MSG_LOGINCOMING);
	}
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

			_LOG_DEBUG_("[Data Response] uid=[%d]\n", otherplayer->id);
			_LOG_DEBUG_("[Data Response] name=[%s]\n", otherplayer->name);
			_LOG_DEBUG_("[Data Response] ustatus=[%d]\n", otherplayer->status);
			_LOG_DEBUG_("[Data Response] tab_index=[%d]\n", otherplayer->tab_index);
			_LOG_DEBUG_("[Data Response] money=[%lu]\n", otherplayer->money);
			_LOG_DEBUG_("[Data Response] carrycoin=[%lu]\n", otherplayer->carrycoin);
			_LOG_DEBUG_("[Data Response] json=[%s]\n", otherplayer->json);
			sendnum++;
		}
	}
	response.End();
	if(HallManager::getInstance()->sendToHall(player->hallid, &response, false) < 0)
		_LOG_ERROR_("[EnterPrivateRoomProc] Send To Uid[%d] Error!\n", player->id);
	else
		_LOG_DEBUG_("[EnterPrivateRoomProc] Send To Uid[%d] Success\n", player->id);
	return 0;
}

int EnterPrivateRoomProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	if(pt==NULL)
	{
       _LOG_ERROR_("[EnterPrivateRoomProc:doResponse]Context is NULL\n");	
	   return -1;
	}
	if(pt->client == NULL)
	{
		_LOG_ERROR_("[EnterPrivateRoomProc:doResponse]Context is client NULL\n");	
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
	string password = string(param->password);
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

	_LOG_DEBUG_("[DATA Parse] money=[%ld]\n", money);
	_LOG_DEBUG_("[DATA Parse] safemoney=[%ld]\n", safemoney);
	_LOG_DEBUG_("[DATA Parse] roll=[%d]\n", roll);
	_LOG_DEBUG_("[DATA Parse] roll1=[%d]\n", roll1);
	_LOG_DEBUG_("[DATA Parse] coin=[%d]\n", coin);

	Room* room = Room::getInstance();

	Player* player = room->getAvailablePlayer();
	if(player==NULL)
	{
		_LOG_ERROR_("[Room is full,no seat for player] \n" );
		return sendErrorMsg(hallhandler, cmd, uid, -1,ErrorMsg::getInstance()->getErrMsg(-1),pt->seq);
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
	player->login();
	Table* table = NULL;
	table = room->getTable(realTid);

	if(!player->checkMoney())
	{
		_LOG_ERROR_("UID[%d] money is too small money[%d]\n", uid, player->money);
		sendErrorMsg(hallhandler, cmd, uid, -4,ErrorMsg::getInstance()->getErrMsg(-4),pt->seq);
		player->setEnterTime(time(NULL));
		player->leave();
		return 0;
	}

	if(table == NULL)
	{
		_LOG_ERROR_("[Room is full, no table for player uid[%d] realtid[%d]]\n", uid, realTid );
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),pt->seq);
	}

	//处理当两个人都离开房间房间init了，再进入此房间已经不存在
	if(table->tableName[0] == '\0')
	{
		_LOG_ERROR_("[Private Room is Not exist uid[%d] realtid[%d]]\n", uid, realTid );
		return sendErrorMsg(hallhandler, cmd, uid, -2,ErrorMsg::getInstance()->getErrMsg(-2),pt->seq);
	}

	
	if(table->haspasswd && strcmp(table->getPassword(), password.c_str()) != 0)
	{
		_LOG_ERROR_("uid[%d] tid[%d] getPassword[%s] password[%s] password is not correct\n", uid, realTid, table->getPassword(), password.c_str());
		return sendErrorMsg(hallhandler, cmd, uid, -5,ErrorMsg::getInstance()->getErrMsg(-5),pt->seq);
	}

	if(table->playerComming(player) != 0)
	{
		_LOG_WARN_("Trans Table[%d] realTid[%d] Coming Error, maybe is full Player[%d]\n", tid, realTid, player->id);
		return sendErrorMsg(hallhandler, cmd, uid, -21,ErrorMsg::getInstance()->getErrMsg(-21),pt->seq);
	}

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

	_LOG_INFO_("[EnterPrivateRoomProc] UID=[%d] NorLogin OK ,seat_id[%d]\n", player->id,player->seat_id );
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
	return 0;
}

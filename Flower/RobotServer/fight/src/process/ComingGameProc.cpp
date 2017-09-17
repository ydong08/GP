#include "ComingGameProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Despatch.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "RobotDefine.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

ComingGameProc::ComingGameProc()
{
	this->name = "ComingGameProc";
}

ComingGameProc::~ComingGameProc()
{

} 

int ComingGameProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = dynamic_cast<HallHandler*>(client);
	Player* player = PlayerManager::getInstance().getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_LOGINCOMING, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteString(player->name);
	requestPacket.WriteInt(player->tid);
	requestPacket.WriteInt64(player->money);
	requestPacket.WriteShort(player->clevel);
	requestPacket.WriteString(player->json);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	ULOGGER(E_LOG_DEBUG, player->id) << "name=" << player->name << " money=" << player->money << " currmax=" << player->currmax
		<< " level=" << player->clevel;
	return 0;
}

int ComingGameProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = dynamic_cast<HallHandler*>(clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0 || player == NULL) {
		ULOGGER(E_LOG_INFO, hallHandler->uid) << "[robot exit]";
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int comeid = inputPacket->ReadInt();
	short seatid = inputPacket->ReadShort();
	short playerindex = inputPacket->ReadByte();
	int ante = inputPacket->ReadInt();
	short num = inputPacket->ReadByte();
	ULOGGER(E_LOG_DEBUG, uid) << "ustatus=" << ustatus << " tid=" << tid << " tstatus=" << tstatus << " comeid=" << comeid
		<< " seatid=" << seatid << " playerindex=" << playerindex << " num=" << num;

	if (tid != player->tid) {
		ULOGGER(E_LOG_DEBUG, uid) << "[robot exit] tid=" << tid << " not equal player.tid=" << player->tid;
		return EXIT;
	}
	player->tstatus = tstatus;
	
	if(uid == comeid)
	{
		player->status = ustatus;
		player->seat_id = seatid;
		player->tab_index = playerindex;
	}

	for(int i = 0; i < num; i++)
	{
		int anthorid = inputPacket->ReadInt();
		string name = inputPacket->ReadString();
		short status = inputPacket->ReadShort();
		short playeridx = inputPacket->ReadByte();
		int64_t money = inputPacket->ReadInt64();
		string json = inputPacket->ReadString();
		if(uid == anthorid)
		{
			strcpy(player->name, name.c_str());
			player->money = money;
			strcpy(player->json, json.c_str());
			Json::Reader reader;
			Json::Value value;
			if(reader.parse(player->json, value))
			{
				player->source = pase_json(value,"source",0);
			}
		}
		else
		{
			player->player_array[playeridx].id = anthorid;
			player->player_array[playeridx].status = status;
			strcpy(player->player_array[playeridx].name, name.c_str());
			player->player_array[playeridx].money = money;
			strcpy(player->player_array[playeridx].json, json.c_str());
			Json::Reader reader;
			Json::Value value;
			if(reader.parse(player->player_array[playeridx].json, value))
			{
				player->player_array[playeridx].source = pase_json(value,"source",0);
			}
		}
	}
	if(uid == comeid)
	{
		player->startHeartTimer(uid, ROBOT_HEARTBEAT_TIMEOUT);
		return CLIENT_MSG_START_GAME;
	}
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_LOGINCOMING, ComingGameProc)
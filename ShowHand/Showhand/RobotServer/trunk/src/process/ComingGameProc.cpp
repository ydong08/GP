#include "ComingGameProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Options.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
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

	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
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
	/*printf("Send ComingGameProc Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->name=[%s]\n", "robot");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->money=[%ld]\n", player->money);
	printf("Data Send: player->clevel=[%d]\n", player->clevel);*/
	return 0;
}

int ComingGameProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv ComingGameProc Packet From Server\n");
	printf("Data Recv: retcode=[%d]\n",retcode);
	printf("Data Recv: retmsg=[%s]\n",retmsg.c_str());*/
	if(retcode < 0 || player == NULL)
	{
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int comeid = inputPacket->ReadInt();
	short seatid = inputPacket->ReadShort();
	short playerindex = inputPacket->ReadShort();
	short num = inputPacket->ReadShort();
	/*printf("Data Recv: uid=[%d]\n",uid);
	printf("Data Recv: ustatus=[%d]\n",ustatus);
	printf("Data Recv: tid=[%d]\n",tid);
	printf("Data Recv: tstatus=[%d]\n",tstatus);
	printf("Data Recv: comeid=[%d]\n",comeid);
	printf("Data Recv: seatid=[%d]\n",seatid);
	printf("Data Recv: playerindex=[%d]\n",playerindex);
	printf("Data Recv: num=[%d]\n\n",num);*/
	if(tid != player->tid)
		return EXIT;
	player->status = tstatus;
	
	if(uid == comeid)
	{
		if(num > 3)
			return EXIT;
		player->status = ustatus;
		player->seat_id = seatid;
		player->tab_index = playerindex;
	}

	for(int i = 0; i < num; i++)
	{
		int anthorid = inputPacket->ReadInt();
		string name = inputPacket->ReadString();
		short status = inputPacket->ReadShort();
		short playeridx = inputPacket->ReadShort();
		int64_t money = inputPacket->ReadInt64();
		int64_t carrycoin = inputPacket->ReadInt64();
		string json = inputPacket->ReadString();
		/*printf("Data Recv: anthorid=[%d]\n",anthorid);
		printf("Data Recv: name=[%s]\n",name.c_str());
		printf("Data Recv: status=[%d]\n",status);
		printf("Data Recv: playeridx=[%d]\n",playeridx);
		printf("Data Recv: money=[%ld]\n",money);
		printf("Data Recv: carrycoin=[%ld]\n",carrycoin);
		printf("Data Recv: json=[%s]\n\n",json.c_str());*/
		if(uid == anthorid)
		{
			strcpy(player->name, name.c_str());
			player->money = money;
			player->carrycoin = carrycoin;
			strcpy(player->json, json.c_str());
			Json::Reader reader;
			Json::Value value;
			if(reader.parse(player->json, value))
			{
				player->source = pase_json(value,"source",0);
			}

			if(player->bactive)
			{
				player->startActiveLeaveTimer(3+rand()%4);
			}
		}
		else
		{
			player->player_array[playeridx].id = anthorid;
			player->player_array[playeridx].status = status;
			strcpy(player->player_array[playeridx].name, name.c_str());
			player->player_array[playeridx].money = money;
			player->player_array[playeridx].carrycoin = carrycoin;
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
		player->startHeartTimer(uid, 30);
		return CLIENT_MSG_START_GAME;
	}
	return 0;
}


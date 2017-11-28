#include "SendCardProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Options.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>
using namespace std;

SendCardProc::SendCardProc()
{
	this->name = "SendCardProc";
}

SendCardProc::~SendCardProc()
{

} 

int SendCardProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int SendCardProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv SendCardProc Packet From Server\n");
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
	int firstbetuid = inputPacket->ReadInt();
	short firstoptype = inputPacket->ReadShort();
	BYTE basecard = inputPacket->ReadByte();
	short currRound = inputPacket->ReadShort();
	int64_t limitcoin = inputPacket->ReadInt64();
	short num = inputPacket->ReadShort(); 
	printf("Data Recv: uid=[%d]\n",uid);
	printf("Data Recv: ustatus=[%d]\n",ustatus);
	printf("Data Recv: tid=[%d]\n",tid);
	printf("Data Recv: tstatus=[%d]\n",tstatus);
	printf("Data Recv: firstbetuid=[%d]\n",firstbetuid);
	printf("Data Recv: firstoptype=[%d]\n",firstoptype);
	printf("Data Recv: limitcoin=[%ld]\n",limitcoin);
	player->status = tstatus;
	if(tid != player->tid)
		return EXIT;
	
	if(uid == firstbetuid)
	{
		player->diffcoin = 0;
		player->optype = firstoptype;
		player->limitcoin = limitcoin;
		player->card_array[0] = basecard;
	}
	else
	{
		player->diffcoin = 0;
		player->card_array[0] = basecard;
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(player->player_array[i].id == firstbetuid)
			{
				player->player_array[i].optype = firstoptype;
				player->player_array[i].limitcoin = limitcoin;
				break;
			}
		}
	}	
	player->currRound = currRound;
	Player * otherPlayer = NULL;
	for(int i = 0; i < num; i++)
	{
		short indexplayer = -1;
		int anthorid = inputPacket->ReadInt();
		short status = inputPacket->ReadShort();
		short playeridx = inputPacket->ReadShort();
		int64_t betcount = inputPacket->ReadInt64();
		short hascard = inputPacket->ReadShort();
		printf("Data Recv: anthorid=[%d]\n",anthorid);
		for(int j = 0; j < GAME_PLAYER; ++j)
		{
			if(player->player_array[j].id == anthorid)
			{
				indexplayer = j;
				break;
			}
		}
		if(player->id == anthorid)
			otherPlayer = player;
		if(hascard == 1)
		{
			if(indexplayer != -1)
				player->player_array[indexplayer].hascard = true;
			else
				otherPlayer->hascard = true;
			//当前还没有到所有要亮底牌的时候
			if(currRound != 6)
			{
				//如果第五轮则发牌也只是发明牌的
				if(currRound == 5)
				{
					for(int j = 1; j < 5; j++)
					{
						if(indexplayer != -1)
						{
							player->player_array[indexplayer].card_array[j] = inputPacket->ReadByte();
						}
						else
							otherPlayer->card_array[j] = inputPacket->ReadByte();
					}
				}
				else
				{
					for(int j = 1; j < currRound + 1; j++)
					{
						if(indexplayer != -1)
						{
							player->player_array[indexplayer].card_array[j] = inputPacket->ReadByte();
						}
						else
							otherPlayer->card_array[j] = inputPacket->ReadByte();
					}
				}
				if(indexplayer != -1)
					 BYTE currcardvalue = inputPacket->ReadShort();
				else
					otherPlayer->currcardvalue = inputPacket->ReadShort(); 
			}
			else
			{
				//如果是第六轮直接把所有玩家的底牌都发过去
				for(int j = 0; j < 5; j++)
				{
					if(indexplayer != -1)
					{
						player->player_array[indexplayer].card_array[j] = inputPacket->ReadByte();
					}
					else
						otherPlayer->card_array[j] = inputPacket->ReadByte();
				}
				if(indexplayer != -1)
				{
					BYTE curr = inputPacket->ReadShort(); 
				}
				else
				{
					otherPlayer->currcardvalue = inputPacket->ReadShort(); 
					otherPlayer->finalcardvalue = otherPlayer->currcardvalue;
				}
			}
		}
		if(indexplayer == -1)
			otherPlayer->betCoinList[0] = betcount;
	}
	if(uid == firstbetuid)
	{
		player->startBetCoinTimer(uid, 5);
		//_LOG_DEBUG_("===SendCard===uid:[%d]===== startBetCoinTimer\n", uid);
	}
	return 0;
}



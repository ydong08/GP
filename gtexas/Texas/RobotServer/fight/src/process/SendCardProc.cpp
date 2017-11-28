#include "SendCardProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
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
	BYTE card1 = inputPacket->ReadByte();
	BYTE card2 = inputPacket->ReadByte();
	int firstbetuid = inputPacket->ReadInt();
	short firstoptype = inputPacket->ReadShort();
	int64_t roundCoin = inputPacket->ReadInt64();
	int64_t countCoin = inputPacket->ReadInt64();
	int64_t limitcoin = inputPacket->ReadInt64();
	int64_t currmax = inputPacket->ReadInt64();
	short currRound = inputPacket->ReadByte();
	printf("Data Recv: uid=[%d]\n",uid);
	printf("Data Recv: ustatus=[%d]\n",ustatus);
	printf("Data Recv: tid=[%d]\n",tid);
	printf("Data Recv: tstatus=[%d]\n",tstatus);
	printf("Data Recv: firstbetuid=[%d]\n",firstbetuid);
	printf("Data Recv: firstoptype=[%d]\n",firstoptype);
	printf("Data Recv: limitcoin=[%ld]\n",limitcoin);
	printf("Data Recv: currmax=[%ld]\n",currmax);
	printf("Data Recv: currRound=[%d]\n",currRound);
	printf("Data Recv: uid=[%d] card1=[0x%02x] card2=[0x%02x]=====\n", uid, card1, card2);

	player->currMaxCoin = currmax;

	player->status = tstatus;
	if(tid != player->tid)
		return EXIT;

	player->card_array[0] = card1;
	player->card_array[1] = card2;
	if(uid == firstbetuid)
	{
		player->optype = firstoptype;
		player->limitcoin = limitcoin;
		player->card_array[0] = card1;
		player->card_array[1] = card2;
	}
	player->currRound = currRound;

	player->betCoinList[currRound] = roundCoin;
	player->betCoinList[0] = countCoin;

	BYTE center1,center2,center3,center4,center5;
	center1 = center2 = center3 = center4 = center5 = 0;
	
	if(currRound == 1)
	{
		printf("handcardtype:%d\n",inputPacket->ReadByte());
	}
	if(currRound == 2)
	{
		center1 = inputPacket->ReadByte();
		center2 = inputPacket->ReadByte();
		center3 = inputPacket->ReadByte();
		printf("Data Recv: center1=[%02x]\n", center1);
		printf("Data Recv: center2=[%02x]\n", center2);
		printf("Data Recv: center3=[%02x]\n", center3);
		short hascard = inputPacket->ReadByte();
		printf("hascard2:%d\n",hascard);
		if(hascard == 1)
		{
			for(int j =0; j < 5; j++)
			{
				inputPacket->ReadByte();
			}
			printf("handcardtype2:%d\n",inputPacket->ReadByte());
		}
	}
	if(currRound == 3)
	{
		center1 = inputPacket->ReadByte();
		center2 = inputPacket->ReadByte();
		center3 = inputPacket->ReadByte();
		center4 = inputPacket->ReadByte();
		printf("Data Recv: center1=[%02x]\n", center1);
		printf("Data Recv: center2=[%02x]\n", center2);
		printf("Data Recv: center3=[%02x]\n", center3);
		printf("Data Recv: center4=[%02x]\n", center4);
		short hascard = inputPacket->ReadByte();
		printf("hascard3:%d\n",hascard);
		if(hascard == 1)
		{
			for(int j =0; j < 5; j++)
			{
				inputPacket->ReadByte();
			}
			printf("handcardtype3:%d\n",inputPacket->ReadByte());
		}
	}
	if(currRound == 4)
	{
		center1 = inputPacket->ReadByte();
		center2 = inputPacket->ReadByte();
		center3 = inputPacket->ReadByte();
		center4 = inputPacket->ReadByte();
		center5 = inputPacket->ReadByte();
		printf("Data Recv: center1=[%02x]\n", center1);
		printf("Data Recv: center2=[%02x]\n", center2);
		printf("Data Recv: center3=[%02x]\n", center3);
		printf("Data Recv: center4=[%02x]\n", center4);
		printf("Data Recv: center5=[%02x]\n", center5);
		short hascard = inputPacket->ReadByte();
		printf("hascard4:%d\n",hascard);
		if(hascard == 1)
		{
			for(int j =0; j < 5; j++)
			{
				inputPacket->ReadByte();
			}
			printf("handcardtype4:%d\n",inputPacket->ReadByte());
		}
	}
	if(currRound == 5)
	{
		center1 = inputPacket->ReadByte();
		center2 = inputPacket->ReadByte();
		center3 = inputPacket->ReadByte();
		center4 = inputPacket->ReadByte();
		center5 = inputPacket->ReadByte();
		printf("Data Recv: center1=[%02x]\n", center1);
		printf("Data Recv: center2=[%02x]\n", center2);
		printf("Data Recv: center3=[%02x]\n", center3);
		printf("Data Recv: center4=[%02x]\n", center4);
		printf("Data Recv: center5=[%02x]\n", center5);
	}
	player->CenterCard[0] = center1;
	player->CenterCard[1] = center2;
	player->CenterCard[2] = center3;
	player->CenterCard[3] = center4;
	player->CenterCard[4] = center5;
	if(uid == firstbetuid)
	{
		player->startBetCoinTimer(uid, 2);
		//_LOG_DEBUG_("===SendCard===uid:[%d]===== startBetCoinTimer\n", uid);
	}
	return 0;
}



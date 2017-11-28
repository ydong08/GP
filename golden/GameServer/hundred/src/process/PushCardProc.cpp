

#include "PushCardProc.h"
#include "Logger.h"
#include "Room.h"
#include <vector>
#include "ProcessManager.h"
#include "GameCmd.h"
#include "IProcess.h"

REGISTER_PROCESS(PHP_PUSH_CARD, PushCardProc)


PushCardProc::PushCardProc()
{
	this->name = "PushCardProc";
}

PushCardProc::~PushCardProc()
{
}

static void replyPhp(CDLSocketHandler * clientHandler, int cmd, short retcode)
{
	OutputPacket response;
	response.Begin(cmd, 0);
	response.WriteShort(retcode);
	response.End();
	clientHandler->Send(response.packet_buf(), response.packet_size());
}

int PushCardProc::doRequest(CDLSocketHandler * clientHandler, InputPacket * inputPacket, Context * pt)
{
	int cmd = inputPacket->GetCmdType();
	int bankerCardType = inputPacket->ReadInt();	//庄家牌型
	int tianCardType = inputPacket->ReadInt();		//天牌型
	int diCardType = inputPacket->ReadInt();		//地牌型
	int xuanCardType = inputPacket->ReadInt();		//玄牌型
	int huangCardType = inputPacket->ReadInt();		//黄牌型

	LOGGER(E_LOG_INFO) << "recieve php push card msg, banker = " << bankerCardType
		<< " tian = " << tianCardType
		<< " di = " << diCardType
		<< " xuan = " << xuanCardType
		<< " huang = " << huangCardType;

	Room* room = Room::getInstance();
	Table *table = room->getTable();
	if (table == NULL)
	{
		LOGGER(E_LOG_ERROR) << "table is NULL";
		replyPhp(clientHandler, cmd, 1);	// reply php operate failed!
		return 0;
	}
	// reply php operate success!
	replyPhp(clientHandler, cmd, 0);

	std::vector<BYTE> cards;
	cards.push_back(bankerCardType);
	cards.push_back(tianCardType);
	cards.push_back(diCardType);
	cards.push_back(xuanCardType);
	cards.push_back(huangCardType);
	table->ReceivePushCardType(cards);

	return 0;
}

int PushCardProc::doResponse(CDLSocketHandler * /*clientHandler*/, InputPacket * /*inputPacket*/, Context * /*pt*/)
{
	return 0;
}

#include "PushCardProc.h"
#include "Logger.h"
#include "Room.h"
#include "GameCmd.h"
#include "ProcessManager.h"

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
	int xiandui = inputPacket->ReadInt();	//赢的区域
	int xian = inputPacket->ReadInt();		//是否有庄对
	int he = inputPacket->ReadInt();		//是否有闲对
	int zhuang = inputPacket->ReadInt();		//是否有闲对
	int zhuangdui = inputPacket->ReadInt();		//是否有闲对

	int winArea = WIN_TYPE_NONE;
	if (xian == 1)
	{
		winArea = WIN_TYPE_XIAN;
	}
	if (he == 1)
	{
		winArea = WIN_TYPE_HE;
	}
	if (zhuang == 1)
	{
		winArea = WIN_TYPE_ZHUANG;
	}

	LOGGER(E_LOG_INFO) << "recieve php push card msg, win area = " << winArea
		<< " zhuangdui = " << zhuangdui
		<< " xiandui = " << xiandui
		;

	//Room* room = Room::getInstance();
	//Table *table = room->getTable();
	//if (table == NULL)
	//{
	//	LOGGER(E_LOG_ERROR) << "table is NULL";
	//	replyPhp(clientHandler, cmd, 1);	// reply php operate failed!
	//	return 0;
	//}
	//// reply php operate success!
	//replyPhp(clientHandler, cmd, 0);
	//table->receivePush(winArea, zhuangdui, xiandui);

	return 0;
}

REGISTER_PROCESS(PHP_PUSH_CARD, PushCardProc);
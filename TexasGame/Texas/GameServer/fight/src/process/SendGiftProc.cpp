#include "SendGiftProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ErrorMsg.h"
#include "ProcessManager.h"
#include "ProtocolServerId.h"
#include "GameCmd.h"
#include <string>
#include "IProcess.h"
#include "Player.h" 
#include "Table.h"

using namespace std;

SendGiftProc::SendGiftProc()
{
	this->name = "SendGiftProc";
}

SendGiftProc::~SendGiftProc()
{

} 

int SendGiftProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt )
{
	//_NOTUSED(pt);
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	short source = pPacket->GetSource();
	_NOTUSED(source);
	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
    BYTE sendtoall = pPacket->ReadByte();
    int toseatid = pPacket->ReadInt();
    short type = pPacket->ReadShort();
    int subid = pPacket->ReadInt();
    int price = pPacket->ReadInt();
	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;

	_LOG_INFO_("==>[SendGiftProc]  cmd[0x%04x] uid[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] svid=[%d] reallTid[%d] toseatid[%d] type=[%d] subid[%d] price[%d]\n",
                tid, svid, realTid, toseatid, type, subid, price);
	Room* room = Room::getInstance();

	Table* table = room->getTable(realTid);

	if (table == NULL)
	{
		_LOG_ERROR_("[SendGiftProc] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n", uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -2, ErrorMsg::getInstance()->getErrMsg(-2), seq);
	}

    if (price < 0 || price > 10000)
    {
        _LOG_ERROR_("[SendGiftProc] uid=[%d] tid=[%d] realTid=[%d] price not correct\n", uid, tid, realTid);
        return sendErrorMsg(clientHandler, cmd, uid, -26, ErrorMsg::getInstance()->getErrMsg(-26), seq);
    }

	Player* player = table->getPlayerInTab(uid);
	if (player == NULL)
	{
		_LOG_ERROR_("[SendGiftProc] uid=[%d] tid=[%d] realTid=[%d] Your not in This Table\n", uid, tid, realTid);
		return sendErrorMsg(clientHandler, cmd, uid, -1, ErrorMsg::getInstance()->getErrMsg(-1), seq);
	}

	if ((toseatid < 0 || toseatid > GAME_PLAYER) || !table->player_array[toseatid]) {
		_LOG_ERROR_("[SendGiftProc] uid=[%d] tid=[%d] realTid=[%d] toseatid[%d] not in This Table\n", uid, tid, realTid, toseatid);
		return sendErrorMsg(clientHandler, cmd, uid, -22, ErrorMsg::getInstance()->getErrMsg(-22), seq);
	}

	player->setActiveTime(time(NULL));
    table->SendGift(player, price, sendtoall);

	IProcess::sendGiftInfo(table, player, toseatid, type, subid, sendtoall, seq);
	return 0;
}

int SendGiftProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_SEND_GIFT, SendGiftProc)
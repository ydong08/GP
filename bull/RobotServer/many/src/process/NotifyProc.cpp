

#include <string>

#include "NotifyProc.h"
#include "PlayerManager.h"

NotifyProc::NotifyProc()
{
	this->name = "NotifyProc";
}

NotifyProc::~NotifyProc()
{
}

int NotifyProc::doRequest(CDLSocketHandler * /*clientHandler*/, InputPacket * /*inputPacket*/, Context * /*pt*/)
{
	return 0;
}

int NotifyProc::doResponse(CDLSocketHandler * clientHandler, InputPacket * inputPacket, Context * pt)
{
	int error_code = inputPacket->ReadShort();
	std::string error_mag = inputPacket->ReadString();
	if (error_code < 0)
	{
		LOGGER(E_LOG_ERROR) << "error code = " << error_code << " error msg = " << error_mag;
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	int vipuid = inputPacket->ReadInt();
	int vip_play_count[BETNUM] = { 0 };
	int64_t vip_bet_array[BETNUM] = { 0 };
	for (int i = 1; i < BETNUM; i++)
	{
		vip_play_count[i] = inputPacket->ReadShort();
		vip_bet_array[i] = inputPacket->ReadInt64();
	}
	int bankerid = inputPacket->ReadInt();
	int64_t banker_money = inputPacket->ReadInt64();
	for (int i = 1; i < BETNUM; i++)
	{
		int rate = PlayerManager::getInstance()->SavePlayerBetRecord(vipuid, i, vip_bet_array[i], vip_play_count[i]);
		PlayerManager::getInstance()->updateAreaBetLimit(banker_money, i, rate);
	}
	return 0;
}

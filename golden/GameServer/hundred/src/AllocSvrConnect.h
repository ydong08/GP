

#pragma once


#include "BaseAllocConnect.h"

class Player;
class Table;

class AllocSvrConnect : public BaseAllocConnect
{
public:
	static AllocSvrConnect* getInstance();
	virtual ~AllocSvrConnect();

	int reportSeverInfo();
	int updateSeverInfo();

	int userLeaveGame(Player* player, int uid = 0);
	int userEnterGame(Player* player);
	int userUpdateStatus(Player* player, short nm_nStatus);
	int updateTableUserCount(Table* table);
	int updateTableStatus(Table* table);
	int applyMatchPlayer(Table* table);

	int processReportResponse(InputPacket* pPacket);
	int processUpdateResponse(InputPacket* pPacket);
	int processSetServerStatus(InputPacket* pPacket);
	int processSeverInfoResponse(InputPacket* pPacket);
	int processCreatePrivateTable(InputPacket* pPacket);

	virtual int processOtherMsg(InputPacket* pPacket);

private:
	AllocSvrConnect();
};
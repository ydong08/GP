#ifndef AllocSvrConnect_H
#define AllocSvrConnect_H

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
	int setSeverStatus();
	int getSeverInfo();

	int userLeaveGame(Player* player, int uid = 0);
	int userEnterGame(Player* player);
	int userUpdateStatus(Player* player, short nm_nStatus);
	int updateTableUserCount(Table* table);
	int updateTableStatus(Table* table);

	int trumptToUser(short type, const char* msg, short pid);

	int processReportResponse(InputPacket* pPacket);
	int processUpdateResponse(InputPacket* pPacket);
	int processSetServerStatus(InputPacket* pPacket);
	int processSeverInfoResponse(InputPacket* pPacket);
	int processCreatePrivateTable(InputPacket* pPacket);

private:
	AllocSvrConnect();
};

#endif

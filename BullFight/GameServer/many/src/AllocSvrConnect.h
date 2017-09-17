#ifndef AllocSvrConnect_H
#define AllocSvrConnect_H
#include "CDL_Timer_Handler.h"
#include "CDLSocketHandler.h"
#include "Packet.h"
#include "DLDecoder.h"
#include "Room.h"
#include "BaseAllocConnect.h"

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

		int processReportResponse(InputPacket* pPacket);
		int processUpdateResponse(InputPacket* pPacket);
		int processSetServerStatus(InputPacket* pPacket);
		int processSeverInfoResponse(InputPacket* pPacket);
		int processCreatePrivateTable(InputPacket* pPacket);

		virtual int processOtherMsg(InputPacket* pPacket);

	private:
		AllocSvrConnect();

};

#endif

#ifndef __CreateRoomProc_H__
#define __CreateRoomProc_H__

#include <string>
#include "BaseProcess.h"
#include "json/json.h"
#include "ProtocolServerId.h"
#include "IRoomCardTable.h"

class Table;

struct CreateRoomParams{
    int m_ReturnCode;
    char m_ReturnMsg[256];
    int m_Command;
    int m_Sequence;
    int m_Source;
    int m_GameID;
    int m_GameLevel;
    int m_JoinType;
    int m_TableID;
    int m_PlayerID;
    int64_t m_lRoomCard;
    char m_JsonOptions[512];
    int m_HallID;

    CreateRoomParams() : m_ReturnCode(0)
            , m_Command(0)
            , m_Sequence(0)
            , m_Source(E_MSG_SOURCE_UNKONW)
            , m_GameID(0)
            , m_GameLevel(0)
            , m_JoinType(0)
            , m_TableID(0)
            , m_PlayerID(0)
            , m_lRoomCard(0)
            , m_HallID(0)
    {

    }
};

class CreateRoomProc : public BaseProcess
{
public:
	CreateRoomProc();
	virtual ~CreateRoomProc();

	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);
	virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt);

protected:
	int process();

	int parseDynamicInfo(std::string& jsonOptions, int &actualMatchCount, bool &isCharged, RoomCardMember& rcmem);
	void packCommonFields(OutputPacket& response, short retCode, const char *retMsg, Table *table);
	string packOptionFields(Table* pTable);

protected:
	int sendNormalMsgInternal(short retCode, const char *retMsg, Table *table);
	int sendErrorMsgInternal(short errcode, const char *errMsg = NULL);

public:
	Table * getTable(int tableID);
	int arrangeTable(Table** ppDestTable, string jsonOptions, string gameData, int ownerId, int createTime, int tableID = -1);

protected:
    CreateRoomParams *params;
};

#endif // !__CreateRoomProc_H__

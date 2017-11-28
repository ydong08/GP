

#include "PlayerEmotionProc.h"
#include "HallManager.h"
#include "Logger.h"
#include "Room.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "IProcess.h"
#include "GameCmd.h"
#include "ProcessManager.h"


PlayerEmotionProc::PlayerEmotionProc()
{
    this->name = "PlayerEmotionProc";
}

PlayerEmotionProc::~PlayerEmotionProc()
{
    
}

int PlayerEmotionProc::doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt)
{
    char src = inputPacket->GetSource();
    int cmd = inputPacket->GetCmdType();
    int gameid = inputPacket->GetOptType();
    
    int uid = inputPacket->ReadInt();
    int tid = inputPacket->ReadInt();
	short realTid = tid & 0x0000FFFF;
    short seatid = inputPacket->ReadShort();
    short emotionid = inputPacket->ReadShort();
    ULOGGER(E_LOG_INFO, uid) << "seat id = " << seatid << " emotion id = " << emotionid;
    Table* tab = Room::getInstance()->getTable(realTid);
    if (tab == NULL)
    {
        ULOGGER(E_LOG_ERROR, uid) << "table is NULL!";
        return sendErrorMsg(clientHandler, cmd, uid, -1, ErrorMsg::getInstance()->getErrMsg(-1));
    }
    Player* player = tab->getPlayer(uid);
    if (player == NULL)
    {
        ULOGGER(E_LOG_ERROR, uid) << "player is NULL!";
        return sendErrorMsg(clientHandler, cmd, uid, -1, ErrorMsg::getInstance()->getErrMsg(-1));
    }
    for (int i = 0; i < GAME_PLAYER; i++)
    {
        Player* p = tab->player_array[i];
        if (p != NULL)
        {
            OutputPacket response;
            response.Begin(cmd, p->id);
            response.WriteShort(0);
            response.WriteString("ok");
            response.WriteInt(p->id);
            response.WriteInt(uid);
            response.WriteShort(seatid);
            response.WriteShort(emotionid);
            response.End();
            HallManager::getInstance()->sendToHall(p->m_nHallid, &response, false);
        }
    }
    return 0;
}


REGISTER_PROCESS(CLIENT_MSG_EMOTION, PlayerEmotionProc);

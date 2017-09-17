#include "HeartProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "BackConnect.h"

HeartProc::HeartProc()
{
    this->name = "HeartProc";
}

HeartProc::~HeartProc()
{

} 

int HeartProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
    int cmd = pPacket->GetCmdType() ;

    if(cmd==HALL_HEART_BEAT)
    {
        int id =  pPacket->ReadInt();
        string msg = pPacket->ReadString();
        //_LOG_DEBUG_("[Recv Data] id=[%d]\n", id );
        //_LOG_DEBUG_("[Recv Data] msg=[%s]\n",msg.c_str() );
        return 0;
    }
    
    int uid = pPacket->ReadInt();
    if(uid<0)
    {
        _LOG_ERROR_("Transfer HeartProc request to Back_UserServer Error, uid:%d is invalid\n", uid );
        return -1;
    }

    if(BackConnectManager::getNodes(uid%10 + 1)->send(pPacket)>0)
    {
        _LOG_DEBUG_("Transfer HeartProc request to Back_UserServer OK\n" );
        return 0;
    }
    else
    {
        _LOG_ERROR_("Transfer HeartProc request to Back_UserServer Error\n" );
        return -1;
    }
    return 0;
}

int HeartProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
    _NOTUSED(clientHandler);
    _NOTUSED(inputPacket);
    _NOTUSED(pt);
    return 0;
}

/*__attribute__ ((constructor)) void add_heart() 
{
    printf("[%s]\n",__FILE__);
}*/

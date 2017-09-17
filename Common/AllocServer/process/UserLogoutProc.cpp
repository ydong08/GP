#include "UserLogoutProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "BackConnect.h"

UserLogoutProc::UserLogoutProc()
{
    this->name = "UserLogoutProc";
}

UserLogoutProc::~UserLogoutProc()
{

} 

int UserLogoutProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{    
    _NOTUSED(client);
    _NOTUSED(pt);
    int uid = pPacket->ReadInt();
    if(uid > 1000)
        _LOG_INFO_("[UserLogoutProc] uid:%d logout\n", uid);
    if(uid<0)
    {
        _LOG_ERROR_("Transfer UserLogoutProc request to Back_UserServer Error, uid:%d is invalid \n", uid );
        return -1;
    }

    if(BackConnectManager::getNodes(uid%10 + 1)->send(pPacket)>0)
    {
        _LOG_DEBUG_("Transfer UserLogoutProc request to Back_UserServer OK\n" );
        return 0;
    }
    else
    {
        _LOG_ERROR_("Transfer UserLogoutProc request to Back_UserServer Error\n" );
        return -1;
    }
}

int UserLogoutProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
    _NOTUSED(clientHandler);
    _NOTUSED(inputPacket);
    _NOTUSED(pt);
    return 0;
}


#include "UserLoginProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "BackConnect.h"

UserLoginProc::UserLoginProc()
{
    this->name = "UserLoginProc";
}

UserLoginProc::~UserLoginProc()
{

} 

int UserLoginProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
    _NOTUSED(client);
    short seq = pPacket->GetSeqNum();
    //把当前包的序列号设置为当前进程的序列号
    pPacket->SetSeqNum(pt->seq);
    //这里要把当前这个包的seq放到回调里面，方便继续给前端发送包
    pt->seq = seq;
    
    int uid = pPacket->ReadInt();
    if(uid<0)
    {
        _LOG_ERROR_("Transfer uid:[%d] is invalid, UserLoginProc request to Back_UserServer Error\n",uid );
        return -1;
    }
    if(BackConnectManager::getNodes(uid%10 + 1)->send(pPacket)>0)
    {
        _LOG_DEBUG_("Transfer[%d] UserLoginProc request to Back_UserServer OK\n",uid );
        return 1;
    }
    else
    {
        _LOG_ERROR_("Transfer[%d] UserLoginProc request to Back_UserServer Error\n",uid );
        return -1;
    }
    return 0;
}

int UserLoginProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
    _NOTUSED(clientHandler);
    if(pt==NULL)
    {
       _LOG_ERROR_("[UserLoginProc:doResponse]Context is NULL\n");    
       return -1;
    }
    //printf("============inputPacket->GetUid():%d\n", inputPacket->GetUid());
    //short cmd = inputPacket->GetCmdType() ;
    OutputPacket response ;
    response.Copy(inputPacket->packet_buf(),inputPacket->packet_size());
    response.SetSeqNum(pt->seq);//把此包的序列号重新置回先前传入的seq
    response.End();    
    if(pt->client)
    {
        int n = this->send(pt->client, &response, false);
        _LOG_DEBUG_("Send to Hall  success, len=[%d]\n", n);
    }
    else
        _LOG_DEBUG_("pt->client == NULL\n" );
    return 0;
}


#ifndef _GetAllUserStatus_H_
#define _GetAllUserStatus_H_


#include "BaseProcess.h"

class Table;

class Player;

class GetAllUserStatus : public BaseProcess {
public:
    GetAllUserStatus() {};

    virtual ~GetAllUserStatus() {};

    virtual int doRequest(CDLSocketHandler *CDLSocketHandler, InputPacket *inputPacket, Context *pt);
};

#endif




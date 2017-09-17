#ifndef _GetPlayInfo_H_
#define _GetPlayInfo_H_


#include "BaseProcess.h"

class GetPlayInfo : public BaseProcess {
public:
    GetPlayInfo();

    ~GetPlayInfo();

    virtual int doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);
};

#endif




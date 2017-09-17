#ifndef _GetBankerListProc_H_
#define _GetBankerListProc_H_

#include "BaseProcess.h"

class GetBankerListProc : public BaseProcess {
public:
    GetBankerListProc();

    virtual ~GetBankerListProc();

    virtual int doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);
};

#endif


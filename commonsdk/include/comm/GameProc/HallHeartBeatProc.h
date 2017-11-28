#ifndef _HallHeartBeatProc_H_
#define _HallHeartBeatProc_H_

#include "BaseProcess.h"

class HallHeartBeatProc : public BaseProcess {
public:
    HallHeartBeatProc();

    virtual ~HallHeartBeatProc();

    virtual int doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);
};

#endif


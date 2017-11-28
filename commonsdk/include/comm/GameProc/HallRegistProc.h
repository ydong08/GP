#ifndef _HallRegistProc_H_
#define _HallRegistProc_H_

#include "BaseProcess.h"

class HallRegistProc : public BaseProcess {
public:
    HallRegistProc();

    virtual ~HallRegistProc();

    virtual int doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);
};

#endif


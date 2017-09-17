#ifndef _ComingGameProc_H_
#define _ComingGameProc_H_

#include "BaseProcess.h"

class GetMoneyProc : public BaseProcess {
public:
    GetMoneyProc();

    virtual ~GetMoneyProc();

    virtual int doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);
};

#endif


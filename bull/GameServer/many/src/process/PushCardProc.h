#pragma once

#include "BaseProcess.h"

class PushCardProc : public BaseProcess {
public:
    PushCardProc();

    virtual ~PushCardProc();

    virtual int doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);
};


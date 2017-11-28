#ifndef _PhpReChargeProc_H_
#define _PhpReChargeProc_H_

#include "BaseProcess.h"

class PhpReChargeProc : public BaseProcess {
public:
    PhpReChargeProc();

    virtual ~PhpReChargeProc();

    virtual int doResponse(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);
};

#endif


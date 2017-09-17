#ifndef _UpdateMoneyProc_H_
#define _UpdateMoneyProc_H_


#include "BaseProcess.h"

class UpdateMoneyProc : public BaseProcess {
public:
    UpdateMoneyProc() {};

    virtual ~UpdateMoneyProc() {};

    int doResponse(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);
};

#endif




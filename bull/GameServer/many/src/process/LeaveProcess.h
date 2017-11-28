#ifndef _LeaveProcess_H_
#define _LeaveProcess_H_

#include "BaseProcess.h"
#include "Table.h"
#include "Player.h"

class LeaveProcess : public BaseProcess {
public:
    LeaveProcess();

    virtual ~LeaveProcess();

    virtual int doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);

private:
    int sendLeaveInfo(Table *table, Player *leavePlayer, short seq);
};

#endif


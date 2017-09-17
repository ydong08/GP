#ifndef _LogComingProc_H_
#define _LogComingProc_H_

#include "BaseProcess.h"
#include "Table.h"
#include "Player.h"

class LogComingProc : public BaseProcess {
public:
    LogComingProc();

    virtual ~LogComingProc();

    virtual int doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);

    virtual int doResponse(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);

private:
    int sendTabePlayersInfo(Table *table, Player *player, short seq);
};

#endif


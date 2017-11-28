#ifndef _CancleBankerProc_H_
#define _CancleBankerProc_H_

#include "BaseProcess.h"
#include "Player.h"
#include "Table.h"

class CancleBankerProc : public BaseProcess {
public:
    CancleBankerProc();

    virtual ~CancleBankerProc();

    virtual int doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);

private:
    int sendTabePlayersInfo(Player *player, Table *table, Player *applyer, short seq);
};

#endif


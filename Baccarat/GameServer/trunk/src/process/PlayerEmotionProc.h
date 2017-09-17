

#pragma once

#include "BaseProcess.h"


class PlayerEmotionProc : public BaseProcess
{
public:
    PlayerEmotionProc();
    virtual ~PlayerEmotionProc();
    virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt);
};

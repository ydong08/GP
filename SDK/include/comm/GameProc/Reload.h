#ifndef _Reload_H_
#define _Reload_H_

#include "BaseProcess.h"

class Reload : public BaseProcess {
public:
    Reload() { this->name = "Reload"; };

    virtual ~Reload() {};

    virtual int doRequest(CDLSocketHandler *clientHandler, InputPacket *inputPacket, Context *pt);
};

#endif




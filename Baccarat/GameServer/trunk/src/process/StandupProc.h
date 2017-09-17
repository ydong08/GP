#ifndef __StandupProc_H__
#define __StandupProc_H__

#include "BaseProcess.h"

class StandupProc : public BaseProcess
{
public:
	StandupProc();
	virtual ~StandupProc();

	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket, Context* pt);
};

#endif // !__StandupProc_H__

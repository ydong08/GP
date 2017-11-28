#ifndef _GsTransToUserServer_H_
#define _GsTransToUserServer_H_

#include "IProcess.h"
#include "ProcessFactory.h"

class GsTransToUserServer :public IProcess
{
	public:
		GsTransToUserServer();
		virtual ~GsTransToUserServer();
		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  ;
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )   ; 
};

#endif


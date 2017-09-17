#ifndef _GetSvrCfg_H_
#define _GetSvrCfg_H_


#include "../IProcess.h"
class Table;
class Player;

class GetSvrCfg  : public IProcess
{
	public:
		GetSvrCfg(){};
		virtual ~GetSvrCfg(){};
	 
	 	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt );
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt ) ;       

	private:
		int sendErrorMsg(CDLSocketHandler* clientHandler, short errcode, char* errmsg) ;
		
};
#endif




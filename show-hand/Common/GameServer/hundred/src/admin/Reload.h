#ifndef _Reload_H_
#define _Reload_H_


#include "BaseProcess.h"
class Table;
class Player;

class Reload  : public BaseProcess
{
	public:
		Reload(){};
		virtual ~Reload(){};
	 	
	 	virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt );
		virtual int doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt ) ;       

	private:
		int sendErrorMsg(CDLSocketHandler* clientHandler, short errcode, char* errmsg) ;

		int ReloadScore(CDLSocketHandler* clientHandler, InputPacket* inputPacket);
		int ReloadTableLua(CDLSocketHandler* clientHandler, InputPacket* inputPacket);
		int ReloadErrorMsg(CDLSocketHandler* clientHandler, InputPacket* inputPacket);
		int SetLogServerReport(CDLSocketHandler* clientHandler, InputPacket* inputPacket);
		int ReloadObConf(CDLSocketHandler* clientHandler, InputPacket* inputPacket);
		int SetObSwitch(CDLSocketHandler* clientHandler, InputPacket* inputPacket);
};
#endif




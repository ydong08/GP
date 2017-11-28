#ifndef _LogoutProcess_H_
#define _LogoutProcess_H_

#include "BaseProcess.h"

class Table;
class Player;

class LogoutProcess:public BaseProcess
{
	public:
		LogoutProcess();
		virtual ~LogoutProcess();

		virtual int doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt )  ;

	private:
		int sendPlayersLogoutInfo(Table* table,Player* leavePlayer);
};

#endif

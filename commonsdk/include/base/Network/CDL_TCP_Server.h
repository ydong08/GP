#ifndef _CDL_TCP_Server_H_
#define _CDL_TCP_Server_H_
#include <string.h>
#include "poller.h"
#include "net.h"
#include "CDL_TCP_Handler.h"

class CDL_TCP_Server : public CPollerObject 
{
	public:
		CDL_TCP_Server( ){};
		virtual ~CDL_TCP_Server( ){};

	protected:
		virtual CDL_TCP_Handler* CreateHandler(int netfd, struct sockaddr_in* peer) = 0;

};



#endif


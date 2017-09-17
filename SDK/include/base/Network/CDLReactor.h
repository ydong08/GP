#ifndef  _CDLReactor_H_
#define  _CDLReactor_H_

#include "timerlist.h"
#include "poller.h"
#include "Singleton.h"
#include "CDL_TCP_Server.h"
#include "CDL_TCP_Handler.h"
#include "CDL_UDP_Server.h"
#define MAX_POLLER 102400
class CDL_Timer_Handler;
 
class CDLReactor
{
	public:
		static CDLReactor* Instance (void);
		static void Destroy (void);
		int Init(int maxpoller=MAX_POLLER);
		int RunEventLoop(); 
		int RegistServer(CDL_TCP_Server* server) ;		  
		int RegistClient(CDL_TCP_Handler* client);
		int RegistServer(CDL_UDP_Server* server) ;		
		int Connect(CDL_TCP_Handler* handler, const char* host, short port, int timeout=0);	
		int RegistTimer(CDL_Timer_Handler* timer, long t);	
		void Quit() { _quit = true; }
 	  
	private:

		CDLReactor();
		virtual ~CDLReactor() ;

		 int AttachPoller (CPollerObject* poller);  

		CPollerUnit*    _pollerunit;
		CTimerUnit*    _timerunit;
		int _maxpoller;
		friend class  CreateUsingNew<CDLReactor>;

		bool _quit;

};
#endif


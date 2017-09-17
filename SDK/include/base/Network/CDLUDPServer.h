#ifndef  _CDLUDPServer_
#define  _CDLUDPServer_

#include "cache.h"
#include "CDL_UDP_Server.h"
using namespace comm::sockcommu;

#define MAX_UDP_RECV_LEN            102400

class CDLUDPServer: public CDL_UDP_Server
{
	public:
		CDLUDPServer();
		virtual ~CDLUDPServer();

		virtual int Listen(const char* bindIp,uint16_t port);
		virtual int InputNotify();
		virtual int Send(const char * buff, int len,char* dst_ip,int dst_port);
		inline string GetIP(){return this->_bindAddr;}
		inline uint16_t GetPort(){return this->_bindPort;}; 

	protected:
		uint16_t	_bindPort;
		char	_bindAddr[128];

	private:
		int handle_input();
		int generate_addr(char *domain, int port, struct sockaddr_in *addr);


		CRawCache       _r;
		CRawCache       _w;
};

#endif



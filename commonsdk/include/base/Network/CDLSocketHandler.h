#ifndef  _CDLSocketHandler_H_
#define _CDLSocketHandler_H_
#include "cache.h"
#include "CDL_Decoder.h"
#include "CDL_TCP_Handler.h"
#include "CDL_Timer_Handler.h"
using namespace comm::sockcommu;

#define MAX_WEB_RECV_LEN          10240 
#define MAX_SEQ_LEN				  512

enum CConnState
{
    CONN_IDLE,
    CONN_FATAL_ERROR,
    CONN_DATA_ERROR,
    CONN_CONNECTING,
    CONN_DISCONNECT,
    CONN_CONNECTED,
    CONN_DATA_SENDING,
    CONN_DATA_RECVING,
    CONN_SEND_DONE,
    CONN_RECV_DONE,
    CONN_APPEND_SENDING,
    CONN_APPEND_DONE,
	CONN_DECODE_NULL1,
	CONN_DECODE_NULL2,
	CONN_XML_POLICY,
	CONN_COLSE_CONNECT,
};

// class CDLSocketHandler;

// class CSocketErrorTimer : public CCTimer
// {
// public:
// 	CSocketErrorTimer() {}
// 	~CSocketErrorTimer() {}
// 
// 	void startErrorTimer(CDLSocketHandler *handle);
// 
// 	virtual int ProcessOnTimerOut();
// 
// private:
// 	CDLSocketHandler *m_handle;	
// };

class CDLSocketHandler  : public CDL_TCP_Handler
{
public:
	CDLSocketHandler();
	virtual ~CDLSocketHandler();
 
public:
	virtual int Init();
	virtual int InputNotify();
	virtual int OutputNotify ();
	virtual int HangupNotify ();
	virtual int OnClose();
		
	/*
		>=0 : 成功
		<0  : 网络失败,可以close
		*/
	virtual int Send(const char * buff, int len);
	void CloseConnect();

	class PacketListener
	{
	public:
		virtual ~PacketListener() {}
		virtual bool OnPacketCompleteEx(const char* data, int len) = 0;
	};

	void setPacketListener(PacketListener* listener) { _packetListener = listener; }

protected:		
	virtual int OnConnected() ;
	virtual int OnPacketComplete(const char * data, int len);
	virtual CDL_Decoder*  CreateDecoder()=0;
		
private:
	int handle_input();
	int handle_output();
	void Reset();

	bool		 _bclose;
	CDL_Decoder* _decode;
	CConnState	 _stage;
	CRawCache    _r;
	CRawCache    _w;
	PacketListener* _packetListener;
};

#endif


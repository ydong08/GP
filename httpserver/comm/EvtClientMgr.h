#pragma once

//#include "EvtClient.h"
#include "ClientSocketEx.h"

class CEvtClientMgr
{
public:
	CEvtClientMgr(void);
	~CEvtClientMgr(void);
	void Init();

	CClientSocketEx* GetMoneySvrConn() { return m_pMoneySvrConn; };
    
    CClientSocketEx* GetUserSvrConn() { return m_pUserSvrConn;}
private:
	CClientSocketEx* m_pMoneySvrConn;
    CClientSocketEx* m_pUserSvrConn;
};


#include "EvtClientMgr.h"

#include "../CConfig.h"


CEvtClientMgr::CEvtClientMgr(void)
{
	m_pMoneySvrConn = NULL;
    m_pUserSvrConn = NULL;
}


CEvtClientMgr::~CEvtClientMgr(void)
{
	if(m_pMoneySvrConn)
	{
		delete m_pMoneySvrConn;
		m_pMoneySvrConn = NULL;
	}
    
    if (m_pUserSvrConn)
    {
        delete m_pUserSvrConn;
        m_pUserSvrConn = NULL;
    }
}

void CEvtClientMgr::Init()
{
	if(NULL == m_pMoneySvrConn)
	{
		m_pMoneySvrConn = new CClientSocketEx(CConfig::getInstance().m_MoneySvrIp.c_str(), CConfig::getInstance().m_MoneySvrPort,500000,500000); //, CConfig::getInstance().m_MoneySvrHeartTime
		m_pMoneySvrConn->ReConnect();
	}
    if(NULL == m_pUserSvrConn)
	{
		m_pUserSvrConn = new CClientSocketEx(CConfig::getInstance().m_UserSvrIp.c_str(), CConfig::getInstance().m_UserSvrPort,500000,500000); //, CConfig::getInstance().m_MoneySvrHeartTime
		m_pUserSvrConn->ReConnect();
	}
}

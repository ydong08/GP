#include "UserDoing.h"


CUserDoing::CUserDoing(void)
{
}


CUserDoing::~CUserDoing(void)
{
}

bool CUserDoing::AddUser(uint32_t nMid)
{
	m_mutex.lock();
	std::pair<std::set<uint32_t>::iterator,bool> ret;
	ret = m_setUsers.insert(nMid);
	m_mutex.unlock();
	return ret.second;
}

bool CUserDoing::DelUser(uint32_t nMid)
{
	m_mutex.lock();
	std::set<uint32_t>::iterator it = m_setUsers.find(nMid); 
	bool bRet = false;
	if (it != m_setUsers.end()) 
	{
		m_setUsers.erase(it);
		bRet = true;
	}
	m_mutex.unlock();
	return bRet;
}

CAutoUserDoing::CAutoUserDoing( uint32_t nMid ) : m_nMid(nMid)
{
	m_bInsertOk = CUserDoing::getInstance().AddUser(nMid);
}

CAutoUserDoing::~CAutoUserDoing( void )
{
	CUserDoing::getInstance().DelUser(m_nMid);
}

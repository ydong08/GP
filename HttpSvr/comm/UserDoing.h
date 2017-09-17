#pragma once
#ifndef UserDoing_h__
#define UserDoing_h__


#include <set>
#include <mutex>              // std::mutex, std::unique_lock
#include <stdint.h>

class CUserDoing
{
public:
	CUserDoing(void);
	~CUserDoing(void);
	bool AddUser(uint32_t nMid);
	bool DelUser(uint32_t nMid);

	static CUserDoing& getInstance()
	{
		static CUserDoing inst;
		return inst;
	}
private:
	std::set<uint32_t>		m_setUsers;
	std::mutex				m_mutex;
};

class CAutoUserDoing
{
public:
	CAutoUserDoing(uint32_t nMid);
	~CAutoUserDoing(void);
	bool IsDoing() { return !m_bInsertOk; }
private:
	bool m_bInsertOk;
	uint32_t m_nMid;
};

#endif // UserDoing_h__
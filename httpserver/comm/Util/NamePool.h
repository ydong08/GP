#ifndef _NAME_POOL_H_
#define _NAME_POOL_H_

#include <string>
#include <vector>
#include <map>

class NamePool
{
public:
	static NamePool* getInstance();
	~NamePool();

	bool Initialize(const char *lpFileName);

	const char* AllocName();
	void FreeName(const char* pName);

private:
	NamePool();

private:
	struct Item
	{
		std::string	m_strName;
		bool		m_bUsed;

		Item()
		{
			m_strName = "";
			m_bUsed = false;
		}

		Item(const Item &other)
		{
			m_strName = other.m_strName;
			m_bUsed = other.m_bUsed;
		}

		Item& operator=(const Item &other)
		{
			if (this != &other)
			{
				m_strName = other.m_strName;
				m_bUsed = other.m_bUsed;
			}

			return *this;
		}
	};

	typedef std::vector<Item> NameArray;
	typedef std::map<std::string, int> NameMap;
	typedef NameMap::iterator MapIt;

private:
	int				m_nCount;
	int				m_nUsed;
	int				m_nCur;
	std::string		m_strEmpty;
	NameArray		m_names;
	NameMap			m_maps;
};



#endif


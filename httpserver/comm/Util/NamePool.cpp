#include "NamePool.h"
#include "Logger.h"
#include "Util.h"

#include <memory.h>

#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <math.h>

static NamePool* instance = NULL;

NamePool* NamePool::getInstance()
{
	if(instance==NULL)
	{
		instance = new NamePool();
		instance->Initialize("../conf/names.txt");
	}
	return instance;
}

NamePool::NamePool()
{
	m_strEmpty = "Empty";
}

NamePool::~NamePool()
{
    
}

bool NamePool::Initialize(const char *lpFileName)
{
	std::ifstream			inFile;

	if ((NULL == lpFileName) || (0 == strlen(lpFileName)))
		return false;

	inFile.open(lpFileName);
	if (!inFile.is_open())
		return false;

	std::string				strLine;
	Item					item;

	while (!inFile.eof())
	{
		std::getline(inFile, strLine, '\n');

		if (0 != strLine.length())
		{
			item.m_strName = strLine;
			item.m_bUsed = false;
			m_names.push_back(item);
		}
	}

	m_nCount = (INT)m_names.size();
	m_nUsed = 0;
	m_nCur = 0;
	return true;
}

const char* NamePool::AllocName()
{
	if ((0 == m_nCount) || (m_nCount == m_nUsed))
		return m_strEmpty.c_str();

	int			nIndex = -1;

	while (-1 == nIndex)
	{
		if (!m_names[m_nCur].m_bUsed)
			nIndex = m_nCur;

		m_nCur++;
		if (m_nCur >= m_nCount)
			m_nCur = 0;
	}

	m_nUsed++;
	m_names[nIndex].m_bUsed = true;
	m_maps[m_names[nIndex].m_strName] = nIndex;
	return m_names[nIndex].m_strName.c_str();
}

void NamePool::FreeName(const char* pName)
{
	MapIt			itFind;

	itFind = m_maps.find(std::string(pName));
	if (itFind != m_maps.end())
	{
		m_names[itFind->second].m_bUsed = false;
		m_maps.erase(itFind);
		m_nUsed--;
	}
}
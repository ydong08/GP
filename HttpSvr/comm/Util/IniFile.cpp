#include "IniFile.h"

// -----------------------------------------------------------------------------------------------------

IniFile::IniFile()
{
	m_bOpen = false;
}

// -----------------------------------------------------------------------------------------------------

IniFile::IniFile(const char* lpFileName)
{
	m_bOpen = false;
	m_strFileName = lpFileName ? lpFileName : "";
	OpenFile(m_strFileName);
}

// -----------------------------------------------------------------------------------------------------

IniFile::IniFile(const std::string& strFileName)
{
	m_bOpen = false;
	m_strFileName = strFileName;
	OpenFile(m_strFileName);
}

// -----------------------------------------------------------------------------------------------------

IniFile::~IniFile()
{

}


// -----------------------------------------------------------------------------------------------------

void IniFile::OpenFile(const std::string &strFileName)
{
	std::ifstream			inFile;

	if (strFileName.empty())
		return;

	inFile.open(strFileName.c_str());
	if (!inFile.is_open())
		return;

	m_bOpen = true;

	std::vector<std::string>	stringArray;
	std::string					strLine;
	std::size_t					nFind;

	while (!inFile.eof())
	{
		std::getline(inFile, strLine, '\n');

		nFind = strLine.find_first_of('\r');
		if (nFind != std::string::npos)
			strLine.erase(nFind);

		// 空行
		if (strLine.empty())
			continue;

		// 以'#'起始的行为注释
		nFind = strLine.find_first_not_of(" \t");
		if (nFind != std::string::npos && '#' == strLine.at(nFind))
			continue;

		stringArray.push_back(strLine);
	}

	if (stringArray.empty())
		return;

	m_itemMap.clear();
	std::for_each(stringArray.begin(), stringArray.end(), CAnalyzeIni(this));
}

// -----------------------------------------------------------------------------------------------------

void IniFile::Save()
{
	if (!m_itemMap.empty())
		SaveFile(m_strFileName);
}

// -----------------------------------------------------------------------------------------------------

void IniFile::SaveAs(const char* lpFileName)
{
	if (!m_itemMap.empty())
	{
		std::string			strFileName;

		strFileName = lpFileName;
		SaveFile(strFileName);
	}
}

// -----------------------------------------------------------------------------------------------------

const char* IniFile::ReadString(const char* lpDomain, const char* lpName, const char* lpDefault)
{
	ItemMapIt			itFind;
	std::string			strDomain = lpDomain ? lpDomain : "";
	std::string			strName = lpName ? lpName : "";

	std::transform(strDomain.begin(), strDomain.end(), strDomain.begin(), toupper);
	itFind = m_itemMap.find(strDomain);
	if (itFind == m_itemMap.end())
		return lpDefault;

	std::transform(strName.begin(), strName.end(), strName.begin(), toupper);
	for (ItemIt it = itFind->second.begin(); it != itFind->second.end(); it++)
	{
		if (it->m_strName == strName)
			return it->m_strValue.c_str();
	}

	return lpDefault;
}

// -----------------------------------------------------------------------------------------------------

bool IniFile::WriteString(const char* lpDomain, const char* lpName, const char* lpValue)
{
	ItemMapIt			itFind;
	IniItem				item;

	itFind = m_itemMap.find(std::string(lpDomain));
	if (itFind == m_itemMap.end())
	{
		ItemArray		itemArray;

		item.m_strName = lpName;
		item.m_strValue = lpValue;
		itemArray.push_back(item);
		m_itemMap[std::string(lpDomain)] = itemArray;
		return true;
	}
	else
	{
		for (ItemIt it = itFind->second.begin(); it != itFind->second.end(); it++)
		{
			if (it->m_strName == lpName)
			{
				it->m_strValue = lpValue;
				return true;
			}
		}

		item.m_strName = lpName;
		item.m_strValue = lpValue ? lpValue : ("");
		itFind->second.push_back(item);
		return true;
	}
}

// -----------------------------------------------------------------------------------------------------

int IniFile::ReadInt(const char* lpDomain, const char* lpName, int nDefault)
{
	ItemMapIt			itFind;
	std::string			strDomain = lpDomain ? lpDomain : "";
	std::string			strName = lpName ? lpName : "";

	std::transform(strDomain.begin(), strDomain.end(), strDomain.begin(), toupper);
	itFind = m_itemMap.find(strDomain);
	if (itFind == m_itemMap.end())
		return nDefault;

	std::transform(strName.begin(), strName.end(), strName.begin(), toupper);
	for (ItemIt it = itFind->second.begin(); it != itFind->second.end(); it++)
	{
		if (it->m_strName == strName)
			return atoi(it->m_strValue.c_str());
	}

	return nDefault;
}

// -----------------------------------------------------------------------------------------------------

bool IniFile::WriteInt(const char* lpDomain, const char* lpName, int nValue)
{
	char			acValue[32];

	snprintf(acValue, 32, ("%d"), nValue);
	return WriteString(lpDomain, lpName, acValue);
}

// -----------------------------------------------------------------------------------------------------

double IniFile::ReadDouble(const char* lpDomain, const char* lpName, double dDefault)
{
	ItemMapIt			itFind;
	std::string			strDomain = lpDomain ? lpDomain : "";
	std::string			strName = lpName ? lpName : "";

	std::transform(strDomain.begin(), strDomain.end(), strDomain.begin(), toupper);
	itFind = m_itemMap.find(strDomain);
	if (itFind == m_itemMap.end())
		return dDefault;

	std::transform(strName.begin(), strName.end(), strName.begin(), toupper);
	for (ItemIt it = itFind->second.begin(); it != itFind->second.end(); it++)
	{
		if (it->m_strName == lpName)
			return atof(it->m_strValue.c_str());
	}

	return dDefault;
}

// -----------------------------------------------------------------------------------------------------

bool IniFile::WriteDouble(const char* lpDomain, const char* lpName, double dValue)
{
	char			acValue[32];

	snprintf(acValue, 32, ("%f"), dValue);
	return WriteString(lpDomain, lpName, acValue);
}

// -----------------------------------------------------------------------------------------------------

void IniFile::SaveFile(const std::string &strFileName)
{
	std::ofstream			outFile(strFileName.c_str());
	std::string				strDomain;
	std::string				strName;
	std::string				strValue;

	if (!outFile.is_open())
		return;

	for (ItemMapIt itMap = m_itemMap.begin(); itMap != m_itemMap.end(); itMap++)
	{
		outFile<<"["<<itMap->first<<"]"<<std::endl;
		for (ItemIt itItem = itMap->second.begin(); itItem != itMap->second.end(); itItem++)
		{
			outFile<<itItem->m_strName<<" = "<<itItem->m_strValue<<std::endl;
		}
	}
}

// -----------------------------------------------------------------------------------------------------

bool IniFile::ParseBuffer(const char*lpBuffer)
{
	const char					*pStart = lpBuffer;
	const char					*pFind;
	std::vector<std::string>	stringArray;
	std::string					strLine;
	std::size_t					nFind;

	if (NULL == lpBuffer)
		return false;

	pFind = strchr(pStart, '\n');
	while (NULL != pFind)
	{
		strLine.clear();
		strLine.append(pStart, pFind - pStart);
		pStart = pFind + 1;
		pFind = strchr(pStart, '\n');

		nFind = strLine.find_first_of('\r');
		if (nFind != std::string::npos)
			strLine.erase(nFind);

		// 空行
		if (strLine.empty())
			continue;

		// 以'#'起始的行为注释
		nFind = strLine.find_first_not_of(" \t");
		if ((nFind == std::string::npos) || ('#' == strLine.at(nFind)))
			continue;

		stringArray.push_back(strLine);
	}

	if ((NULL == pFind) && (NULL != pStart))
	{
		strLine= pStart;
		
		nFind = strLine.find_first_of('\r');
		if (nFind != std::string::npos)
			strLine.erase(nFind);

		if (!strLine.empty())
		{
			nFind = strLine.find_first_not_of(" \t");
			if ((nFind != std::string::npos) && ('#' != strLine.at(nFind)))
				stringArray.push_back(strLine);
		}
	}

	if (stringArray.empty())
		return false;

	m_itemMap.clear();
	std::for_each(stringArray.begin(), stringArray.end(), CAnalyzeIni(this));
	return true;
}
#ifndef __INIFILE_H__
#define __INIFILE_H__

#include <memory.h>

#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>

class IniFile
{
public:
	IniFile();
	IniFile(const char* lpFileName);
	IniFile(const std::string& strFileName);
	~IniFile();

	bool ParseBuffer(const char*lpBuffer);

	bool IsOpen() { return m_bOpen; }

	void Save();
	void SaveAs(const char* lpFileName);

	const char* ReadString(const char* lpDomain, const char* lpName, const char* lpDefault);
	bool WriteString(const char* lpDomain, const char* lpName, const char* lpValue);

	int ReadInt(const char* lpDomain, const char* lpName, int nDefault);
	bool WriteInt(const char* lpDomain, const char* lpName, int nValue);

	double ReadDouble(const char* lpDomain, const char* lpName, double dDefault);
	bool WriteDouble(const char* lpDomain, const char* lpName, double dValue);

public:
	struct IniItem
	{
		std::string		m_strName;
		std::string		m_strValue;

		IniItem() {}
		IniItem(const IniItem &other)
		{
			m_strName = other.m_strName;
			m_strValue = other.m_strValue;
		}

		IniItem& operator=(const IniItem &other)
		{
			if (this != &other)
			{
				m_strName = other.m_strName;
				m_strValue = other.m_strValue;
			}
			return *this;
		}
	};
	typedef std::vector<IniItem> ItemArray;
	typedef ItemArray::iterator ItemIt;
	typedef std::map<std::string, ItemArray> ItemMap;
	typedef ItemMap::iterator ItemMapIt;

	class CAnalyzeIni
	{
	public:
		CAnalyzeIni(IniFile *pIniFile)
		{
			m_pIniFile = pIniFile;
		}

		void operator()(const std::string &strLine)
		{
			size_t			nFirst = strLine.find('[');
			size_t			nLast = strLine.rfind(']');
			size_t			nSpaceTab = strLine.find_first_not_of(" \t");
			std::string		strLeftPart;
			std::string		strRightPart;
			std::string		strName;
			std::string		strValue;

			if ((std::string::npos != nFirst) && (std::string::npos != nLast) && ((nFirst+1) != nLast) && (nFirst == nSpaceTab))
			{
				m_strDomain = strLine.substr(nFirst+1, nLast-nFirst-1);
				std::transform(m_strDomain.begin(), m_strDomain.end(), m_strDomain.begin(), toupper);
				return;
			}

			if (m_strDomain.empty())
				return;

			nFirst = strLine.find('=');
			if(std::string::npos == nFirst)
				return;

			strLeftPart = strLine.substr(0, nFirst);
			strRightPart = strLine.substr(nFirst+1, std::string::npos);

			nFirst= strLeftPart.find_first_not_of(" \t");
			nLast = strLeftPart.find_last_not_of(" \t");
			if ((std::string::npos == nFirst) || (std::string::npos == nLast))
				return;
			strName = strLeftPart.substr(nFirst, nLast-nFirst+1);
			std::transform(strName.begin(), strName.end(), strName.begin(), toupper);

			nFirst = strRightPart.find_first_not_of(" \t");
			if
				(((nLast = strRightPart.find("\t#", nFirst )) != std::string::npos) ||
				((nLast = strRightPart.find(" #", nFirst )) != std::string::npos) ||
				((nLast = strRightPart.find("\t//", nFirst )) != std::string::npos)||
				((nLast = strRightPart.find(" //", nFirst )) != std::string::npos))
			{
				strRightPart = strRightPart.substr(0, nLast-nFirst);
			}
			nLast = strRightPart.find_last_not_of(" \t");
			if(nFirst == std::string::npos || nLast == std::string::npos)
			{
				strRightPart.clear();
				strValue.clear();
			}
			else
			{
				strValue = strRightPart.substr(nFirst, nLast-nFirst+1);
			}

			m_pIniFile->WriteString(m_strDomain.c_str(), strName.c_str(), strValue.c_str());
			return ;
		}

	public:
		std::string			m_strDomain;
		IniFile				*m_pIniFile;
	};

private:
	void OpenFile(const std::string &strFileName);
	void SaveFile(const std::string &strFileName);

private:
	std::string			m_strFileName;
	bool				m_bOpen;
	ItemMap				m_itemMap;
};

#endif

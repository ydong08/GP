#include "XyCryptoEx.h"
#include "XyCrypto.h"
#include "base64.h"


std::string XyEnCrypt(unsigned char *pData, unsigned long nLen)
{
	if (NULL == pData || 0 == nLen) return "";

	std::string strRet;
	strRet.append((char *)pData,nLen);

	int nXyLen = XyCrypt((unsigned char *)strRet.c_str(), strRet.length());
	char * szTmp = NULL;
	int nB64Len = base64::base64Encode((unsigned char *)strRet.c_str(), strRet.length(),&szTmp);
	if (szTmp)
	{
		strRet.assign(szTmp,nB64Len);
		free(szTmp);
	}
	else
	{
		strRet = "";
	}
	return strRet;
}

std::string XyDeCrypt(unsigned char *pData, unsigned long nLen)
{
	if (NULL == pData || 0 == nLen) return "";
	std::string strRet;
	strRet.append((char *)pData,nLen);
	unsigned char * szTmp = NULL;
	int nB64Len = base64::base64Decode((unsigned char *)strRet.c_str(), strRet.length(),&szTmp);
	if (szTmp)
	{
		strRet.assign((char*)szTmp,nB64Len);
		free(szTmp);

		int nXyLen = XyCrypt((unsigned char *)strRet.c_str(), strRet.length());
		if (-1 == nXyLen) strRet = "";
	}
	else
	{
		strRet = "";
	}

	return strRet;
}

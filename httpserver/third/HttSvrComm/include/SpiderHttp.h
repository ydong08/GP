#pragma once

#include "curl/curl.h"
#include <string>
#include <vector>
using namespace std;

class CSpiderHttp
{
public:
	CSpiderHttp(void);
	virtual ~CSpiderHttp(void);

	void  Get( const string& strUrl, const string& strPostData = "", const string& strOrigin = "", bool bNoBody = false );
	const char* GetBodyBuf() {return m_strBodyBuffer.c_str();}
	unsigned int GetBodyLen() {return m_strBodyBuffer.size();}
	const char* GetHeaderBuf() {return m_strHeadBuffer.c_str();}
	unsigned int GetHeaderLen() {return m_strHeadBuffer.size();}
	void SetConnectTimeout(int sec);
	void SetReadWriteTimeout(int sec);
	void SetServerUserPwd(const char *userpwd);
	void SetProxyUserPwd(const char *userpwd);
	void SetProxy( const char *proxy,unsigned int type );
	void SetEncoding( const char *encoding );
	char *  GetCookie();
	char *  GetContentType();
	void SetGetRange(const char *range);
	void SetUserAgent(const char *agent);

	void SetHttpHead(string strHeadInfo);
	void ClearHttpHead();

	void SetMaxRedir(long MaxRedirs);
	bool IsNetError(long res);

private:
	//fHttpVer : 1.0		1.1			2.0
	void init(float fHttpVer = 1.1);
	void release();
	vector<string> m_vsHeader;
	bool m_bNoBody;
	string m_strBodyBuffer;
	string m_strHeadBuffer;
	CURL* m_curlHandle;

	static size_t write_data(  const void *buffer, size_t size, size_t count, void *stream );
	static size_t write_head(  const void *buffer, size_t size, size_t count, void *stream, const void* host );
};


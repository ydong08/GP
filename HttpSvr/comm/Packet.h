#ifndef _PacketBase_H
#define _PacketBase_H

#include <string>
#include <string.h>
#include <assert.h>
#include "Typedef.h"
//#include "Protocol.h"

#define	DL_TCP_DEFAULT_BUFFER	8192

#define SERVER_PACKET_DEFAULT_VER		1
#define MAX_ENCRYPT_MAP		64		//定义加解密用到的特定数字的数组


#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

//#define GAME_ID 0
extern int GAME_ID;
extern short SOURCE;

#pragma pack(1)
struct  PacketHeader
{
	unsigned int pack_len;     /*整个包的长度，包括包头+包体,不包含自身四个字节*/
	unsigned char version;     /*协议版本号*/
	unsigned short main_cmd;    /*主命令字*/
	unsigned char sub_cmd;     /*子命令字*/
	unsigned char encode;		/*加密类型*/
	unsigned short seq;              /*序列号*/
	unsigned char source;  	/*消息来源 */
	unsigned char type;		/*操作类型*/
	unsigned int uid;		/*用户ID*/
	unsigned char code;     		    /*校验code*/
};
#pragma pack()

uint64_t htonll(uint64_t v);
uint64_t ntohll(uint64_t v);

class PacketBase
{
public:

	enum
	{
		PACKET_HEADER_SIZE = sizeof(PacketHeader),
		PACKET_BUFFER_SIZE = DL_TCP_DEFAULT_BUFFER
	};

	PacketBase(void) {}

	char *packet_buf(void) { return m_strBuf; }
	int packet_size(void) { return m_nPacketSize; }

	/*整个包的长度*/
	short GetBodyLength(void)
	{
		int nLen;
		_readHeader((char*)&nLen, sizeof(int), 0);
		int len = ntohl(nLen);
		return len;
	}

	/*协议版本号*/
	char GetVersion(void)
	{
		char c;
		_readHeader(&c, sizeof(char), 4);	// 
		return c;
	}

	/*主命令字*/
	short GetCmdType(void)
	{
		short nCmdType;
		_readHeader((char*)&nCmdType, sizeof(short), 5);// 
		short cmd = ntohs(nCmdType);
		return cmd;
	}

	/*子命令字*/
	BYTE GetSubCmd(void)
	{
		BYTE subCmd;
		_readHeader((char*)&subCmd, sizeof(BYTE), 7);//
		return subCmd;
	}

	void SetSubCmd(BYTE i)
	{
		_writeHeader((char*)&i, sizeof(BYTE), 7);
	}

	/*加密类型*/
	BYTE GetEnCode(void)
	{
		BYTE encode;
		_readHeader((char*)&encode, sizeof(BYTE), 8);//
		return encode;
	}

	void SetEnCode(BYTE i)
	{
		_writeHeader((char*)&i, sizeof(BYTE), 8);
	}

	/*序列号*/
	short GetSeqNum(void)
	{
		short i;
		_readHeader((char*)&i, sizeof(short), 9);	//
		short seq = ntohs(i);
		return seq;
	}

	/*设置序列号 */
	void SetSeqNum(short seqNum)
	{
		short seq = htons(seqNum);
		_writeHeader((char*)&seq, sizeof(short), 9);
	}

	/*消息来源 */
	char GetSource(void)
	{
		char c;
		_readHeader(&c, sizeof(char), 11);	//  
		return c;
	}

	void SetSource(BYTE i)
	{
		_writeHeader((char*)&i, sizeof(BYTE), 11);
	}

	/*此包的操作类型*/
	BYTE GetOptType(void)
	{
		BYTE opttype;
		_readHeader((char*)&opttype, sizeof(BYTE), 12);//
		return opttype;
	}

	void SetOptType(BYTE i)
	{
		_writeHeader((char*)&i, sizeof(BYTE), 12);
	}

	/*用户ID*/
	int GetUid(void)
	{
		int uid;
		_readHeader((char*)&uid, sizeof(int), 13);// 
		int Uid = ntohl(uid);
		return Uid;
	}

	void SetUid(int uid)
	{
		int nuid = htonl(uid);
		_writeHeader((char*)&nuid, sizeof(int), 13);
	}


	/*校验code*/
	BYTE GetCheckCode(void)
	{
		BYTE code;
		_readHeader((char*)&code, sizeof(BYTE), 17);//
		return code;
	}




protected:
	////////////////////////////////////////////////////////////////////////////////
	bool _copy(const void *pInBuf, int nLen)
	{
		if (nLen > PACKET_BUFFER_SIZE)
			return false;

		_reset();
		memcpy(m_strBuf, pInBuf, nLen);
		m_nPacketSize = nLen;
		return true;
	}
	////////////////////////////////////////////////////////////////////////////////
	void _begin(short nCmdType, int uid, char cVersion)
	{
		_reset();
		short cmd = htons(nCmdType);
		uid = htonl(uid);
		char gameid = GAME_ID;
		char sourceType = SOURCE;
		_writeHeader((char*)&cVersion, sizeof(char), 4);	// 版本号
		_writeHeader((char*)&cmd, sizeof(short), 5);		// 命令码
		_writeHeader((char*)&sourceType, sizeof(char), 11);			// 消息来源
		_writeHeader((char*)&gameid, sizeof(char), 12);	    // 游戏ID
		_writeHeader((char*)&uid, sizeof(int), 13);	        // UID
	}
	void _SetBegin(short nCmdType)
	{
		short cmd = htons(nCmdType);
		_writeHeader((char*)&cmd, sizeof(short), 5);// 
	}



	void _end()
	{
		int nBody = m_nPacketSize - 4;	//数据包长度包括命令头和body,4个字节是数据包长度
		int len = htonl(nBody);
		_writeHeader((char*)&len, sizeof(int), 0);	// 包正文长度
		BYTE code = 0;
		_writeHeader((char*)&code, sizeof(BYTE), 17);	//效验码 
	}
	/////////////////////////////////////////////////////////////////////////////////
	void _reset(void)
	{
		memset(m_strBuf, 0, PACKET_BUFFER_SIZE);
		m_nBufPos = PACKET_HEADER_SIZE;
		m_nPacketSize = PACKET_HEADER_SIZE;
	}
	// 
	bool _Read(char *pOut, int nLen)
	{
		if ((nLen + m_nBufPos) > m_nPacketSize || (nLen + m_nBufPos) > PACKET_BUFFER_SIZE)
			return false;

		memcpy(pOut, m_strBuf + m_nBufPos, nLen);
		m_nBufPos += nLen;
		return true;
	}
	// 
	bool _ReadDel(char *pOut, int nLen)
	{
		if (!_Read(pOut, nLen))
			return false;
		memcpy(m_strBuf + m_nBufPos - nLen, m_strBuf + m_nBufPos, PACKET_BUFFER_SIZE - m_nBufPos);
		m_nBufPos -= nLen;
		m_nPacketSize -= nLen;
		_end();
		return true;
	}
	//
	void _readundo(int nLen)
	{
		m_nBufPos -= nLen;
	}
	//
	char *_readpoint(int nLen)
	{
		if ((nLen + m_nBufPos) > m_nPacketSize)
			return NULL;
		char *p = &m_strBuf[m_nBufPos];
		m_nBufPos += nLen;
		return p;

	}
	//  
	bool _Write(const char *pIn, int nLen)
	{
		if ((m_nPacketSize < 0) || ((nLen + m_nPacketSize) > PACKET_BUFFER_SIZE))
			return false;
		memcpy(m_strBuf + m_nPacketSize, pIn, nLen);
		m_nPacketSize += nLen;
		return true;
	}
	// 
	bool _Insert(const char *pIn, int nLen)
	{
		if ((nLen + m_nPacketSize) > PACKET_BUFFER_SIZE)
			return false;
		memcpy(m_strBuf + PACKET_HEADER_SIZE + nLen, m_strBuf + PACKET_HEADER_SIZE, m_nPacketSize - PACKET_HEADER_SIZE);
		memcpy(m_strBuf + PACKET_HEADER_SIZE, pIn, nLen);
		m_nPacketSize += nLen;
		_end();
		return true;
	}
	// 
	bool _writezero(void)
	{
		if ((m_nPacketSize + 1) > PACKET_BUFFER_SIZE)
			return false;
		memset(m_strBuf + m_nPacketSize, '\0', sizeof(char));
		m_nPacketSize++;
		return true;
	}
	// readHeader
	void _readHeader(char *pOut, int nLen, int nPos)
	{
		if (nPos > 0 || nPos + nLen < PACKET_HEADER_SIZE)
		{
			memcpy(pOut, m_strBuf + nPos, nLen);
		}
	}
	// writeHeader
	void _writeHeader(char *pIn, int nLen, int nPos)
	{
		if (nPos > 0 || nPos + nLen < PACKET_HEADER_SIZE)
		{
			memcpy(m_strBuf + nPos, pIn, nLen);
		}
	}

private:
	char m_strBuf[PACKET_BUFFER_SIZE];
	int m_nPacketSize;
	int m_nBufPos;
};

class InputPacket : public PacketBase
{
public:
	typedef PacketBase base;

	int ReadInt(void) { int nValue = 0; base::_Read((char*)&nValue, sizeof(int)); return ntohl(nValue); } //ʼ
	int64_t ReadInt64(void) { int64_t nValue = 0; base::_Read((char*)&nValue, sizeof(int64_t)); return ntohll(nValue); }
	int ReadIntDel(void) { int nValue = 0; base::_ReadDel((char*)&nValue, sizeof(int)); return ntohl(nValue); }
	short ReadShort(void) { short nValue = 0; base::_Read((char*)&nValue, sizeof(short)); return ntohs(nValue); }
	BYTE ReadByte(void) { BYTE nValue = 0; base::_Read((char*)&nValue, sizeof(BYTE)); return nValue; }

	InputPacket() {};
	InputPacket(const char* data, int len)
	{
		base::_copy(data, len);
	};

	bool ReadString(char *pOutString, int nMaxLen)
	{
		int nLen = ReadInt();
		if (nLen == -1)
			return false;
		if (nLen > nMaxLen)
		{
			base::_readundo(sizeof(short));
			return false;
		}
		return base::_Read(pOutString, nLen);
	}

	char *ReadChar(void)
	{
		int nLen = ReadInt();

		if (nLen == -1)
			return NULL;
		return base::_readpoint(nLen);
	}

	std::string ReadString(void)
	{
		char *p = ReadChar();
		return (p == NULL ? "" : p);
	}

	int ReadBinary(char *pBuf, int nMaxLen)
	{
		int nLen = ReadInt();
		if (nLen == -1)
			return -1;
		if (nLen > nMaxLen)
		{
			base::_readundo(sizeof(int));
			return -1;
		}
		if (base::_Read(pBuf, nLen))
			return nLen;
		return 0;
	}

	int ReadPureBinary(char *pBuf, int nLen)
	{
		if (nLen <1 || nLen >= GetBodyLength())
			return -1;
		if (base::_Read(pBuf, nLen))
			return nLen;
		return 0;
	}

	void Reset(void)
	{
		base::_reset();
	}
	bool Copy(const void *pInBuf, int nLen)
	{
		return base::_copy(pInBuf, nLen);
	}
	bool WriteBody(const char *pIn, int nLen)
	{
		return base::_Write(pIn, nLen);
	}

	void Begin(short nCommand, int uid = 0, char cVersion = SERVER_PACKET_DEFAULT_VER)
	{
		base::_begin(nCommand, uid, cVersion);
	}
	void End(void)
	{
		base::_end();
	}

	int CrevasseBuffer();
};


class OutputPacket : public PacketBase
{
	bool m_isCheckCode;
public:
	OutputPacket(void) { m_isCheckCode = false; }

	typedef PacketBase base;

	bool WriteInt(int nValue) { int value = htonl(nValue); return base::_Write((char*)&value, sizeof(int)); }
	bool WriteInt64(int64_t nValue) { int64_t value = htonll(nValue); return base::_Write((char*)&value, sizeof(int64_t)); }
	bool WriteByte(BYTE nValue) { return base::_Write((char*)&nValue, sizeof(BYTE)); }
	bool WriteShort(short nValue) { short value = htons(nValue); return base::_Write((char*)&value, sizeof(short)); }
	bool InsertInt(int nValue) { int value = htonl(nValue); return base::_Insert((char*)&value, sizeof(int)); }
	bool InsertByte(BYTE nValue) { return base::_Insert((char*)&nValue, sizeof(BYTE)); }
	bool WriteString(const char *pString)
	{
		int nLen = (int)strlen(pString);
		WriteInt(nLen + 1);
		return base::_Write(pString, nLen) && base::_writezero();
	}

	bool WriteString(const std::string &strDate)
	{
		int nLen = (int)strDate.size();
		WriteInt(nLen + 1);
		return base::_Write(strDate.c_str(), nLen) && base::_writezero();
	}

	bool WriteBinary(const char *pBuf, int nLen)
	{
		WriteInt(nLen);
		return base::_Write(pBuf, nLen);
	}
	bool Copy(const void *pInBuf, int nLen)
	{
		return base::_copy(pInBuf, nLen);
	}
	void Begin(short nCommand, int uid = 0, char cVersion = SERVER_PACKET_DEFAULT_VER)
	{
		base::_begin(nCommand, uid, cVersion);
		m_isCheckCode = false;
	}
	void End(void)
	{
		m_isCheckCode = false;
		base::_end();
	}
	void SetBegin(short nCommand)
	{
		base::_SetBegin(nCommand);
	}
	void WriteCheckCode(BYTE nValue)
	{
		base::_writeHeader((char*)&nValue, sizeof(BYTE), 17);
		m_isCheckCode = true;
	}

	bool IsWriteCheckCode(void)
	{
		return m_isCheckCode;
	}

	WORD EncryptBuffer();
};

#endif


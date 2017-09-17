#ifndef _CRYPT_DECODER_H
#define _CRYPT_DECODER_H 

#include "CDLSocketHandler.h"
#include "CDLStreamDecoder.h"
#include "Packet.h"
#include "Typedef.h"


class CryptDecoder : public CDLStreamDecoder
 {
	Singleton(CryptDecoder);

 public:
	virtual int ParsePacket(char * data, int len);

protected://Œ¥ π”√
    virtual inline int GetHeadLen(const char * data, int len)
    {
		return 0;
    }

    virtual inline  int GetPacketLen(const char * data, int len)
    {
		return 0;
    }

    virtual inline  bool CheckHead(const char * data, int len)
    {
		return true;
    }
 };



#endif



#ifndef _DECODER_H
#define _DECODER_H 

#include "CDLSocketHandler.h"
#include "CDLStreamDecoder.h"
#include "Packet.h"
#include "Typedef.h"


class DLDecoder : public CDLStreamDecoder
 {
	Singleton(DLDecoder);

    protected:

        virtual inline int GetHeadLen(const char * data, int len)
        {
             return sizeof(PacketHeader) ;
        }

        virtual inline  int GetPacketLen(const char * data, int len)
        {
            struct PacketHeader* pHeader = (struct PacketHeader*) data;
            int pkglen = sizeof(int) + ntohl(pHeader->pack_len);
            if(pkglen<=0 || pkglen>8187)
                return -1;
            else
                return pkglen;
        }

        virtual inline  bool CheckHead(const char * data, int len)
        {
            struct PacketHeader* pHeader = (struct PacketHeader*) data;

            int pkglen = sizeof(int) + ntohl(pHeader->pack_len);
            if(pkglen<=0 || pkglen>8187)
                 return false;

             return true;
        }
 };



#endif



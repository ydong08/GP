#ifndef  _CDLStreamDecoder_H_
#define _CDLStreamDecoder_H_

#include "CDL_Decoder.h"
 
class CDLStreamDecoder : public CDL_Decoder
 {
	 public:
		CDLStreamDecoder(){};
		virtual ~CDLStreamDecoder(){};

		virtual int ParsePacket(char * data, int len)
		{
			//不足一个包头，继续接收
			if( len < this->GetHeadLen(data,len) )
				return 0;

			//满足一个包头，判断包头合法性
			if( ! this->CheckHead(  data,   len))
				return -1;

			//满足一个包头，判断整包长合法性
			int pkglen = this->GetPacketLen( data,   len);
			if( pkglen<0 )
			{
				return -1;
			}
			//包体未接收完整
			if(len < pkglen)
			{
				return 0;
			}
			//收到一个完整包
			return pkglen;
		}

		protected:
			virtual int GetHeadLen(const char * data, int len)=0;
			virtual bool CheckHead(const char * data, int len)=0;
			virtual int GetPacketLen(const char * data, int len)=0;
	
 };
 


#endif



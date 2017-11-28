#ifndef _newborn_Base_Result_h_
#define _newborn_Base_Result_h_

#include "BaseHeader.h"
#include "BaseCommand.h"

namespace Comm
{
   class BaseResult
   {
      public:
         enum
         {
            None = 0,
            Ok = 1,
            NotFound = 2, /*not found file*/
            DataIncorrect = 3, /* data format error*/
            VersionNotMatch = 4, /* version not matching*/            
            ServiceUnAvailable = 5,
			InvalidUrl = 6,
         };

      public:
         BaseHeader mHeader;
         Field<uint32_t> mResultCode;

      protected:
         char* mBuff;
         uint32_t mSize;

      public:
         BaseResult()
         {
             mResultCode = None;
        	   mBuff = 0;
             mSize = 0; 
		     }
         ~BaseResult()
         {
         	   if(mBuff)
               delete [] mBuff;
         }
         
         int resultCode() const
         {
            return *mResultCode;
         }

         void setResultCode(int code) { *mResultCode = code; }
         const char* buff() const
         {
            return mBuff;
         }

         const uint32_t size() const
         {
            return mSize;
         }

         char* release(uint32_t& outSize)
         {
            char* outBuf = mBuff;
            outSize = mSize;
            mBuff = NULL;
            mSize = 0;
            return outBuf;
         }

		 template<typename _Type>
		 uint32_t buildBuff(_Type& val, bool buildHttpHeader = true)
		 {
			 assert(mSize == 0);
			 static const char* httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %d\r\n\r\n";
			 uint32_t httpHeaderLen = 0;

			 if (*mResultCode == Ok)
			 {
				 FieldList fileList;
				 val.bind(fileList);

				 uint32_t valSize = fileList.size();
				 uint32_t dataLen = mResultCode.size() + valSize;
				 mSize = mHeader.size() + dataLen;

				 mBuff = new char[mSize + 1024];
				 char* pBuff = mBuff;

				 if (buildHttpHeader)
				 {
#ifdef WIN32
					 httpHeaderLen = _snprintf(pBuff, 1024 /*max size*/, httpHeader, mSize);
#else
					 httpHeaderLen = snprintf(pBuff, 1024 /*max size*/, httpHeader, mSize);
#endif
					 pBuff += httpHeaderLen;
				 }
				 pBuff += mHeader.buildBuff(pBuff,
					 mSize, 
					 dataLen);

				 pBuff += mResultCode.encode(pBuff , mResultCode.size());      
				 fileList.encode(pBuff, valSize);
				 mSize += httpHeaderLen; // buff len
				 BaseCommand::makeCheckSum(mBuff + httpHeaderLen + BaseHeader::HeaderSize,
					 dataLen,
					 mBuff + httpHeaderLen + 12);

			 }
			 else
			 {
				 return buildBuff();
			 }
			 return mSize;
		 }

		 uint32_t buildBuff(bool buildHttpHeader = true)
		 {
			 assert(mSize == 0);
			 static const char* httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %d\r\n\r\n";
			 uint32_t httpHeaderLen = 0;

			 mSize = mResultCode.size() + mHeader.size();

			 mBuff = new char[mSize + 1024];
			 char* pBuff = mBuff;

			 if (buildHttpHeader)
			 {
#if defined(WIN32) && defined(_MSC_VER)
				 httpHeaderLen = _snprintf(pBuff, 1024 /*max size*/, httpHeader, mSize);
#else
				 httpHeaderLen = snprintf(pBuff, 1024 /*max size*/, httpHeader, mSize);
#endif
				 pBuff += httpHeaderLen;
			 }
			 pBuff += mHeader.buildBuff(pBuff,
				 mSize, 
				 mResultCode.size());
			 mResultCode.encode(pBuff, mResultCode.size());
			 mSize += httpHeaderLen; // buff len

			 BaseCommand::makeCheckSum(mBuff + httpHeaderLen + BaseHeader::HeaderSize,
				 mResultCode.size(),
				 mBuff + httpHeaderLen + 12);
			 return mSize;
		 }

         template<typename _Type>
         uint32_t parseBuff(_Type& val, char* buff, uint32_t length)
         {
            if (length < ( mResultCode.size() +  BaseHeader::HeaderSize))
            {
               return 0;
            }

            if (mHeader.parseBuff(buff, BaseHeader::HeaderSize) == 0)
            {
               return 0;
            }

            if (!BaseCommand::isCheckSumOK(buff + BaseHeader::HeaderSize,
                                           *mHeader.mLength,
                                           buff + 12))
            {
               return 0;
            }

            buff += BaseHeader::HeaderSize;
            buff += mResultCode.decode(buff, mResultCode.size());

            if (*mResultCode == Ok)
            {
               FieldList fileList;
               val.bind(fileList);
               uint32_t dataLen = *mHeader.mLength - mResultCode.size();
               if (dataLen > 0 && fileList.decode(buff, dataLen) != dataLen)
               {
                  return 0;
               }
               return length;
            }
            else
            {
               return length;
            }
            return 0;
         }
   };

} // namespace Comm

#endif // _newborn_Base_Result_h_

// Local Variables:
// mode:c++
// c-file-style:"ellemtel"
// c-file-offsets:((case-label . +))
// indent-tabs-mode:nil
// End:

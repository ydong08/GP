#include <stdlib.h>
#include "Packet.h"

static BYTE m_EncryptMap[MAX_ENCRYPT_MAP]=				
{
	0x51,0xA1,0x9E,0xD7,0x1E,0x83,0x1C,0x2D,0xE9,0x77,0x3D,0x13,0x93,0x10,0x45,0xFF,
	0x6D,0xC9,0x20,0x2F,0x1B,0x82,0x1A,0x7D,0xF5,0xCF,0x52,0xA8,0xD2,0xA4,0xB4,0x0B,
	0x31,0x97,0x57,0x19,0x34,0xDF,0x5B,0x41,0x58,0x49,0xAA,0x5F,0x0A,0xEF,0x88,0x01,
	0xDC,0x95,0xD4,0xAF,0x7B,0xE3,0x11,0x8E,0x9D,0x16,0x61,0x8C,0x84,0x3C,0x1F,0x5A
};

uint64_t htonll(uint64_t v) {  
    union { uint32_t lv[2]; uint64_t llv; } u;  
    u.lv[0] = htonl(v >> 32);  
    u.lv[1] = htonl(v & 0xFFFFFFFFULL);  
    return u.llv;  
}  
  
uint64_t ntohll(uint64_t v) {  
    union { uint32_t lv[2]; uint64_t llv; } u;  
    u.llv = v;  
    return ((uint64_t)ntohl(u.lv[0]) << 32) | (uint64_t)ntohl(u.lv[1]);  
} 

int InputPacket::CrevasseBuffer() 
{
	BYTE *pcbDataBuffer = (BYTE *)base::packet_buf() + base::PACKET_HEADER_SIZE;
	WORD wDataSize = base::GetBodyLength() - base::PACKET_HEADER_SIZE + sizeof(int);
	
	BYTE EnCode = base::GetEnCode();
	if(EnCode >= MAX_ENCRYPT_MAP)
		return -1;
	BYTE EnCodeNum = m_EncryptMap[EnCode];
	BYTE CheckCode = base::GetCheckCode();
	for (int i=0;i<wDataSize;i++)
	{
		pcbDataBuffer[i] ^= EnCodeNum;
		CheckCode+=pcbDataBuffer[i];
	}

	if(CheckCode!=0) 
	{
		return -1;
	}

	return wDataSize;
}


WORD OutputPacket::EncryptBuffer()
{	
	if(IsWriteCheckCode())  
		return 0;
	BYTE *pcbDataBuffer = (BYTE *)base::packet_buf() + base::PACKET_HEADER_SIZE;
	WORD wDataSize = base::GetBodyLength() - base::PACKET_HEADER_SIZE + sizeof(int);
	
	BYTE EnCode = rand()%MAX_ENCRYPT_MAP;
	BYTE EnCodeNum = m_EncryptMap[EnCode];
	BYTE CheckCode=0;
	for (WORD i=0;i<wDataSize;i++)
	{
		CheckCode+=pcbDataBuffer[i];
		pcbDataBuffer[i] ^= EnCodeNum;
	}
	base::SetEnCode(EnCode);
	WriteCheckCode(~CheckCode+1);
	return wDataSize;
}


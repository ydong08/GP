#pragma once

#include <string>
#include <stdint.h>


namespace DDCrypt
{

#ifdef __cplusplus
	extern "C" {
#endif
	//************************************
	// Method:    DDEncrypt	消息加密函数
	// FullName:  DDCrypt::DDEncrypt
	// Access:    public 
	// Returns:   int	成功返回加密后消息的长度; NULL == szMsg ret :-1 | 0 == nMsgLen ret:-2 | NULL == authKey ret -2 | strlen(authKey) != 256 ret:-4
	// Qualifier:
	// Parameter: const char * szMsg				需要加密的消息
	// Parameter: const unsigned int nMsgLen		消息长度
	// Parameter: const char * authKey				密钥，长度必须是256
	// Parameter: char*& szRetEncrypt				加密成功，返回加密后的内容,需要手动free，失败返回NULL
	// Parameter: uint64_t nMid						用户的mid , 登录注册时，直接传0，表示用默认的auth key解密， 非零时，表示用该用户自己的auth key解密
	//************************************
	int DDEncrypt(const char* szMsg, const unsigned int nMsgLen, const char* authKey,char*&  szRetEncrypt, uint64_t nMid = 0);

	//************************************
	// Method:    GetCryptMid					获取用户mid值，用于解密数据是获取对应的Auth Key
	// FullName:  DDCrypt::GetCryptMid
	// Access:    public 
	// Returns:   uint64_t
	// Qualifier:
	// Parameter: const char * szMsg
	// Parameter: const unsigned int nMsgLen
	//************************************
	uint64_t GetCryptMid(const char* szMsg, const unsigned int nMsgLen);

	//************************************
	// Method:    DDDecrypt							消息解密函数
	// FullName:  DDCrypt::DDDecrypt
	// Access:    public 
	// Returns:   int								成功返回解密后消息的长度， 负数出错
	// Qualifier:
	// Parameter: const char * szMsg				需要解密的消息
	// Parameter: const unsigned int nMsgLen		需要解密的消息长度
	// Parameter: const char * authKey				密钥，长度必须是256
	// Parameter: char * & szRetDecrypt				解密成功，返回加密后的内容,需要手动free，失败返回NULL
	//************************************
	int DDDecrypt( const char* szMsg, const unsigned int nMsgLen, const char* authKey,char*&  szRetDecrypt );


#ifdef __cplusplus
	}
#endif
		
	//************************************
	// Method:    GetMd5				MD5加密
	// FullName:  DDCrypt::GetMd5
	// Access:    public 
	// Returns:   std::string
	// Qualifier:
	// Parameter: const char * szSrc
	// Parameter: const uint32_t nLen
	//************************************
	std::string GetMd5(const char* szSrc,const unsigned int nLen);

	//************************************
	// Method:    GenerateAuthKey			生成一个256长度的密钥
	// FullName:  DDCrypt::GenerateAuthKey
	// Access:    public 
	// Returns:   std::string				成功返回密钥，失败返回空串
	// Qualifier:
	// Parameter: const char * szSrc		要加密成256长度的原始字符
	// Parameter: const unsigned int nLen	字符长度
	//************************************
	std::string GenerateAuthKey(const char* szSrc,const unsigned int nLen);

	//************************************
	// Method:    GetDefAuthKey				获取默认的密钥，用于登录前的数据加密
	// FullName:  DDCrypt::GetDefAuthKey
	// Access:    public 
	// Returns:   std::string
	// Qualifier:
	//************************************
	std::string GetDefAuthKey();
	
	//************************************
	// Method:    Encrypt
	// FullName:  DDCrypt::Encrypt
	// Access:    public 
	// Returns:   std::string						成功返回加密后消息， 失败返回空串
	// Qualifier:
	// Parameter: const char * szMsg				需要加密的消息
	// Parameter: const unsigned int nMsgLen		消息长度
	// Parameter: const char * authKey				密钥，长度必须是256
	// Parameter: uint64_t nMid						用户的mid , 登录注册时，直接传0，表示用默认的auth key解密， 非零时，表示用该用户自己的auth key解密
	//************************************
	std::string Encrypt(const char* szMsg, const unsigned int nMsgLen, const char* authKey, uint64_t nMid = 0);

	//************************************
	// Method:    Decrypt							消息解密函数
	// FullName:  DDCrypt::Decrypt
	// Access:    public 
	// Returns:   std::string						成功返回解密后消息， 失败返回空串
	// Qualifier:
	// Parameter: const char * szMsg				需要解密的消息
	// Parameter: const unsigned int nMsgLen		需要解密的消息长度
	// Parameter: const char * authKey				密钥，长度必须是256
	//************************************
	std::string  Decrypt( const char* szMsg, const unsigned int nMsgLen, const char* authKey);

} // namespace DDCrypt






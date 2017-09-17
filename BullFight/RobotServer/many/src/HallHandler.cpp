#include "GameCmd.h"
#include "CDLReactor.h"
#include "Despatch.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "PlayerManager.h"

#ifdef CRYPT
#include "DDCrypt.h"
#include "CryptRedis.h"
#endif

using namespace std;

static InputPacket pPacket;
//=================HallHandler=========================
HallHandler::HallHandler()
{
	uid = -1;
}

HallHandler::~HallHandler( )
{
	//this->_decode = NULL;
	uid = -1;
}

int HallHandler::Send(OutputPacket* outPacket, bool isEncrypt)
{
	if (isEncrypt)
		outPacket->EncryptBuffer();

#ifdef CRYPT
	char*	pOutputBuf = NULL;
	int		nOutputLen = 0;

	assert(-1 != uid);

	if (m_strAuthKey.empty())
	{
		Player* pPlayer = PlayerManager::getInstance()->getPlayer(uid);

		assert(pPlayer);
		m_strAuthKey = DDCrypt::GenerateAuthKey(pPlayer->json, strlen(pPlayer->json));
		CCryptRedis::getInstance().SetUserKey((uint64_t)uid, m_strAuthKey);
	}

	assert(!m_strAuthKey.empty());
	nOutputLen = DDCrypt::DDEncrypt(outPacket->packet_buf(), outPacket->GetBodyLength(), m_strAuthKey.c_str(), pOutputBuf, (uint64_t)uid);
	int nRet = CDLSocketHandler::Send((const char*)pOutputBuf, nOutputLen) >= 0 ? 0 : -1;
	free(pOutputBuf);
	return nRet;
#else
	return CDLSocketHandler::Send((const char*)outPacket->packet_buf(), outPacket->packet_size()) >= 0 ? 0 : -1;
#endif
}

int HallHandler::OnConnected()
{
#ifdef CRYPT
	m_strAuthKey.clear();
	CDLSocketHandler::setPacketListener(static_cast<CDLSocketHandler::PacketListener *>(this));
#endif
	LOGGER(E_LOG_INFO)<<"Connect netfd=["<<netfd<<"] Success";
	return 0;
}

int HallHandler::OnClose()
{
	//_LOG_INFO_("Connect Close fd=[%d] Success\n",netfd);
	LOGGER(E_LOG_INFO)<<"Connect Close fd=["<<netfd<<"] Success";
	if(uid != -1)
		PlayerManager::getInstance()->delPlayer(uid, true);
	return 0;
}

int HallHandler::OnPacketComplete(const char* data, int len)
{
#ifdef CRYPT
	assert(false);
	return 0;
#else
	pPacket.Copy(data, len);
	return Despatch::getInstance()->doBackResponse(this, &pPacket);;
#endif
}

bool HallHandler::OnPacketCompleteEx(const char* data, int len)
{
#ifdef CRYPT
	uint64_t		cur_id = DDCrypt::GetCryptMid(data, len);

	if (cur_id != (uint64_t)uid)
		return false;

	char*	pOutputBuf = NULL;
	int		nOutputLen = 0;

	assert(!m_strAuthKey.empty());
	nOutputLen = DDCrypt::DDDecrypt(data, len, m_strAuthKey.c_str(), pOutputBuf);
	assert(nOutputLen > 0);

	pPacket.Copy(pOutputBuf, nOutputLen);
	Despatch::getInstance()->doBackResponse(this, &pPacket);
	free(pOutputBuf);
	return true;
#else
	assert(false);
	return false;
#endif
}
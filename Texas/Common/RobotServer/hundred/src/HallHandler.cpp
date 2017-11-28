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
	LOGGER(E_LOG_INFO) << "hall handler address = " << this;
	uid = -1;
}

int HallHandler::OnConnected()
{
#ifdef CRYPT
	m_strAuthKey.clear();
	CDLSocketHandler::setPacketListener(static_cast<CDLSocketHandler::PacketListener *>(this));
#endif
	LOGGER(E_LOG_INFO) << "Connect netfd = " << netfd << " Success";
	return 0;
}

int HallHandler::OnClose()
{
	LOGGER(E_LOG_INFO) << "Connect Close netfd = " << netfd << " Success";
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


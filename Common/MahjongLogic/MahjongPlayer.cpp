#include "MahjongPlayer.h"
#include "MahjongTable.h"

CMahjongPlayer::CMahjongPlayer()
{
	m_nSeatPos = 0;
}

CMahjongPlayer::~CMahjongPlayer()
{

}

void CMahjongPlayer::Clear()
{

}

void CMahjongPlayer::ResetGameData()
{
	ZeroMemory(m_handCardIndex, sizeof(m_handCardIndex));
	m_listen = false;

	ZeroMemory(m_discardCard, sizeof(m_discardCard));
	m_discardCount = 0;

	m_forbidChihu = false;
	m_forbidPeng = false;

	m_response = false;
	m_action = WIK_NULL;
	m_operateCard = 0;
	m_performAction = WIK_NULL;

	ZeroMemory(m_weaveItems, sizeof(m_weaveItems));
	m_weaveCount = 0;

	m_chihuKind = 0;
	m_chihuRight.SetEmpty();

	m_outCanTing.clear();
	for (uint8_t i = 0; MAX_INDEX; i++)
		m_outCanTingCard[i].clear();
	m_tingCardInfo.clear();
}

bool CMahjongPlayer::EnterTable(CMahjongTable *pTable)
{
	ASSERT(NULL == m_pTable);

	if (pTable->AddPlayer(this))
	{
		m_pTable = pTable;
		return true;
	}
	else
	{
		return false;
	}
}
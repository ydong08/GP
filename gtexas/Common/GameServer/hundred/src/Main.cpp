#include "Configure.h"
#include "Logger.h"
#include "BaseClientHandler.h"
#include "Room.h"
#include "HallManager.h"
#include "GameApp.h"
#include "CoinConf.h"
#include "AllocSvrConnect.h"
#include "game_version.h"

void GetRoundInfoCb(InputPacket* pPacket) {
	LOGGER(E_LOG_DEBUG) << "GetRoundInfoCallback";
	int uid = pPacket->ReadInt();
	short tid = pPacket->ReadShort();
	int win = pPacket->ReadInt();
	int lose = pPacket->ReadInt();
	int tie = pPacket->ReadInt();
	int runaway = pPacket->ReadInt();
	int64_t maxwin = pPacket->ReadInt64();
	int64_t maxvalue = pPacket->ReadInt64();
	_LOG_DEBUG_("[RoundSocketHandle] uid[%d] win[%d] lose[%d] tie[%d] runaway[%d]\n", uid, win, lose, tie, runaway);
	Room* room = Room::getInstance();
	Table* table = room->getTable();
	if (table)
	{
		Player* player = table->getPlayer(uid);
		if (player)
		{
			player->m_nWin = win;
			player->m_nLose = lose;
			player->m_nRunAway = runaway;
			player->m_lMaxWinMoney = maxwin;
			player->m_lMaxCardValue = maxvalue;
		}
		else
			_LOG_ERROR_("[RoundSocketHandle] Can't Find player[%d] in Table[%d]\n", uid, tid);
	}
	else
		_LOG_ERROR_("[RoundSocketHandle] Can't Find this Table[%d] player[%d]\n", tid, uid);
}

typedef SocketServer<BaseClientHandler> GameAcceptor;

class GameServerApp : public GameApp {
public:
	virtual bool InitRoom() {
		Room::getInstance()->init();
		return true;
	}

	virtual CDL_TCP_Server *NewTCPServer() {
		return NewGameAcceptor<GameAcceptor>();
	}
};

int main(int argc, char* argv[])
{
	GameServerApp app;
	app.Register(Configure::getInstance(), CoinConf::getInstance());
	if (!app.InitConfig(argc, argv)) {
		return -1;
	}
	LOGGER(E_LOG_INFO) << "base git version:" << BASE_SHORT_VERSION;
	LOGGER(E_LOG_INFO) << "comm git version:" << COMM_SHORT_VERSION;
	LOGGER(E_LOG_INFO) << "game git version:" << GAME_SHORT_VERSION;
	LOGGER(E_LOG_INFO) << "game build time:" << GAME_BUILD_TIME;
	AllocSvrConnect::getInstance()->connect(Configure::getInstance()->alloc_host.c_str(), Configure::getInstance()->alloc_port);
	RoundServer()->SetGetRoundInfoCallback(GetRoundInfoCb);

	return app.RunLoop();
}

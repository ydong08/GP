#include "Configure.h"
#include "Logger.h"
#include "BaseClientHandler.h"
#include "Room.h"
#include "HallManager.h"
#include "GameApp.h"
#include "CoinConf.h"
#include "AllocSvrConnect.h"
#include "game_version.h"

typedef SocketServer<BaseClientHandler> GameAcceptor;

class GameServerApp : public GameApp
{
public:
	virtual bool InitRoom()
	{
		Room::getInstance()->init();
		return true;
	}

	virtual CDL_TCP_Server *NewTCPServer()
	{
		return NewGameAcceptor<GameAcceptor>();
	}
};

int main(int argc, char* argv[])
{
	GameServerApp app;
	app.Register(Configure::getInstance(), CoinConf::getInstance());
	if (!app.InitConfig(argc, argv))
	{
		return -1;
	}

	LOGGER(E_LOG_INFO) << "base git version:" << BASE_SHORT_VERSION;
	LOGGER(E_LOG_INFO) << "comm git version:" << COMM_SHORT_VERSION;
	LOGGER(E_LOG_INFO) << "game git version:" << GAME_SHORT_VERSION;
	LOGGER(E_LOG_INFO) << "game build time:" << GAME_BUILD_TIME;
	AllocSvrConnect::getInstance()->connect(Configure::getInstance()->alloc_ip, Configure::getInstance()->alloc_port);

	return app.RunLoop();
}

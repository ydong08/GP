#include <sys/stat.h> 
#include <signal.h> 
#include "IniFile.h"
#include "Configure.h"
#include "Logger.h"
#include "Util.h"
#include "Protocol.h"

#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#endif


Configure::Configure() : GameConfig(GAME_TYPE, E_MSG_SOURCE_SERVER, E_CONFIG_ALLOC)
{
}

Configure::~Configure()
{
}

bool Configure::LoadGameConfig()
{
	std::string svname = ReadString("system", "name", "server");
	char tmp[64] = { 0 };
	sprintf(tmp, "%s_%d", svname.c_str(), m_nServerId);
	name = tmp;
	numplayer = ReadInt("system", "numplayer", 1);
	highlimit = ReadInt("system", "highlimit", 10000000);
	mul = ReadInt("system", "mul", 10);
	matchtime = ReadInt("system", "matchtime", 2);
	allowrobot = ReadInt("system", "allowrobot", 0);
	norobot = ReadInt("system", "norobot", 2);

	return true;
}

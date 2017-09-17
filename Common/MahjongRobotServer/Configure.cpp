#include <sys/stat.h> 
#include <signal.h> 
#include "IniFile.h"
#include "Configure.h"
#include "Logger.h"
#include "Version.h"
#include "Util.h"
#include "Protocol.h"


#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#endif

Configure::Configure() : GameConfig(GAME_TYPE, E_MSG_SOURCE_ROBOT, E_CONFIG_ROBOT)
{
}

Configure::~Configure()
{
}

bool Configure::LoadGameConfig()
{
	// system section
	id = m_nServerId;
	std::string svname = ReadString("system", "name", "server");
	char tmp[64] = { 0 };
	sprintf(tmp, "%s_%d", svname.c_str(), id);
	name = tmp;
	head_url = ReadString("system", "head_url", "");
	monitor_time = ReadInt("system", "monitor_time", 10);
	base_bet_time = ReadInt("system", "base_bet_time", 10);
	play_count = ReadInt("system", "play_count", 2);
	enter_time = ReadInt("system", "enter_time", 5);
	level = m_nLevel;
	konw_contrl_level = ReadInt("system", "konw_contrl_level", 2);
	for (int i = 0; i < 4; i++)
	{
		char key[32] = { 0 };
		sprintf(key, "base_money%d", i + 1);
		base_money[i] = ReadInt("system", key, 1000);
		sprintf(key, "rand_money%d", i + 1);
		rand_money[i] = ReadInt("system", key, 5000);
		sprintf(key, "rand_call%d", i + 1);
		rand_call[i] = ReadInt("system", key, 0);
	}
	for (int i = 0; i < 5; i++)
	{
		char key[32] = { 0 };
		sprintf(key, "rand_vip%d", i);
		rand_vip[i] = ReadInt("system", key, 40);
	}
	base_uid = ReadInt("system", "base_uid", 100000);
	rand_uid = ReadInt("system", "rand_uid", 10000);

	////////////////////////////////////////////////////////////
	// hall section
	hall_ip = ReadString("hall", "ip", "");
	hall_port = ReadInt("hall", "port", 0);

	if (!Util::dir_exist(Util::getModulePath() + "/../conf"))
		Util::mkdir(Util::getModulePath() + "/../conf");
	std::string names = Util::getModulePath() + "/../conf/names.txt";
	if (!Util::dir_exist(names))
	{
		FILE *fp = fopen(names.c_str(), "wb");
		if (!fp) {
			LOGGER(E_LOG_ERROR) << "can't write names.txt";
            return false;
		}
        std::string content = ReadRawString("RobotEtc", "names.txt", "");
        fwrite(content.c_str(), content.length(), 1, fp);
        fclose(fp);
	}
	serverxml = ReadString("server.xml", "content", "");

	return true;
}

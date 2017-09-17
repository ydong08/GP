#include <sys/stat.h>
#include <signal.h>
#include "IniFile.h"
#include "Configure.h"
#include "Logger.h"
#include "Protocol.h"
#include "Util.h"

#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#endif

Configure::Configure(): GameConfig(GAME_TYPE, E_MSG_SOURCE_ROBOT, E_CONFIG_ROBOT)
{

}

Configure::~Configure()
{
}

bool Configure::LoadGameConfig()
{
	m_hallIp = ReadString("Hall", "Host", "");
	m_hallPort = ReadInt("Hall", "Port", 0);
	baseMoney1 = ReadInt("BaseMoney", "num1", 1000);
	baseMoney2 = ReadInt("BaseMoney", "num2", 30000);
	baseMoney3 = ReadInt("BaseMoney", "num3", 120000);
	baseMoney4 = ReadInt("BaseMoney", "num3", 2400000);

	randMoney1 = ReadInt("RandMoney", "num1", 5000);
	randMoney2 = ReadInt("RandMoney", "num2", 100000);
	randMoney3 = ReadInt("RandMoney", "num3", 500000);
	randMoney4 = ReadInt("RandMoney", "num4", 2000000);
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


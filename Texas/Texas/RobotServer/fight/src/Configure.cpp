#include "Configure.h"
#include "Logger.h"
#include "Protocol.h"
#include "Util.h"

Configure::Configure() : GameConfig(E_TEXAS_GAME_ID, E_MSG_SOURCE_ROBOT, E_CONFIG_ROBOT)
{

	basebettime = 0;
	level = 0;
	m_robotId = 0;
}

bool Configure::LoadGameConfig()
{
	monitortime = ReadInt("Timer","MonitorTimer",10);
	basebettime = ReadInt("Timer","BaseBetTimer",2);

	baseplayCount = ReadInt("PlayCount","num",2);

	entertime = ReadInt("Coming","entertime",5);

	konwcontrlLevel = ReadInt("KonwLevel","level",2);
	baseMoney1 = ReadInt("BaseMoney","num1",1000);
	baseMoney2 = ReadInt("BaseMoney","num2",30000);
	baseMoney3 = ReadInt("BaseMoney","num3",120000);
	baseMoney6 = ReadInt("BaseMoney","num6",120000);

	randMoney1 = ReadInt("RandMoney","num1",5000);
	randMoney2 = ReadInt("RandMoney","num2",50000);
	randMoney6 = ReadInt("RandMoney","num6",500000);

	swtichWin1 = ReadInt("Swtich","Money1",10000);
	swtichWin2 = ReadInt("Swtich","Money2",10000);
	swtichWin6 = ReadInt("Swtich","Money6",10000);
	swtichOnKnow1 = ReadInt("Swtich","KnowMoney1",10000);
	swtichOnKnow2 = ReadInt("Swtich","KnowMoney2",200000);
	swtichOnKnow6 = ReadInt("Swtich","KnowMoney6",200000);

	baseuid = ReadInt("RandUID","basenum",100000);
	randuid = ReadInt("RandUID","num",10000);

	randknow = ReadInt("RandKnow","num",60);
	randhead = ReadInt("RandKnow","headnum",60);

	std::string conn_hall_host =  ReadString("Hall","Host","NULL");
	strcpy( hall_ip, conn_hall_host.c_str() );
    hall_port = ReadInt("Hall","Port",0);
	if(hall_port == 0)
	{
		printf("hall_port is null please check your conf\n");
		return false;
	}
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


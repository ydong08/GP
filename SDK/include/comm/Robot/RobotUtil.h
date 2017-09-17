

#pragma once

#include <string>
//fight :  levelid = 0
//hundred : 
//01(2) + gameid(8) + levelid(2) + startid(6) + (14)
#define ROBOT_FLAG_OFFSET	(30)
#define GAME_ID_OFFSET		(22)
#define LEVEL_ID_OFFSET		(17)
#define START_ID_OFFSET		(12)
#define MAX_ROBOT_NUMBER	(2048)

#define IS_ROBOT_UID(uid)	(((uid >> ROBOT_FLAG_OFFSET) && 0x01) == 1)

namespace RobotUtil
{
	bool makeRobotInfo(const std::string& headlink, int &sex, std::string &headurl, int &uid);

	int getRobotStartUid(int gameid, int levelid, int startid);

	bool makeRobotNameSex(std::string headlink, std::string &name, int &sex, std::string &headurl);
}
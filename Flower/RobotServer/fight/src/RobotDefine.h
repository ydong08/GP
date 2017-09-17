#ifndef __ROBOT_DEFINE_H_
#define __ROBOT_DEFINE_H_

#define ROBOT_MONITOR_GAME_TIMEOUT	(20 * 1000)	//20S检查一次游戏服务器

#define ROBOT_BASE_BET_TIMEROUT	    (2)         //机器人基本下注时间

#define ROBOT_RAND_BET_TIMEROUT		(3)			//机器人随机下注时间

#define ROBOT_BASE_CHECK_TIMEOUT    (2)			//机器人基本看牌时间

#define ROBOT_RAND_CHECK_TIMEOUT	(4)			//机器人随机看牌时间

#define ROBOT_LEVEL_ROOM_TIMEOUT	(3)			//机器人离开时间

#define ROBOT_ENTER_TABLE_TIME		(2)			//机器人进入桌子时间

#define ROBOT_BASE_PLAY_COUNT		(4)			//机器人最低玩几局

#define ROBOT_RAND_PLAY_COUNT		(5)			//机器人随机加几局

#define LIMIT_FIGHT_ROBOT_NUM       (3)			//战斗系列产品桌子机器人上限

#define LIMIT_ROBOT_CHECK_ROUND		(2)			//几局之后可以看牌

#define ROBOT_RESTART_TIMEOUT       (5)			//等待几秒进行到下一局

#define ROBOT_HEARTBEAT_TIMEOUT     (30)		//机器人心跳

#endif//End for __ROBOT_TIMEOUT_H_

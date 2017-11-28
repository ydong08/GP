#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

//所有服务器类型
enum E_SERVER_TYPE {
	E_OUT_GAME_ID						= 0,	//不在游戏中
    E_SHOWHAND_GAME_ID					= 1,	//梭哈
	E_BULLFIGHT_GAME_ID					= 2,	//牛牛
	E_LANDLORD_GAME_ID					= 3,	//斗地主
    E_FRUIT_GAME_ID						= 4,	//水果机
	E_DRAGONTIGER_GAME_ID 				= 5,	//龙虎斗
	E_TEXAS_GAME_ID						= 6,	//德州
	E_FLOWER_GAME_ID					= 7,	//炸金花
	E_WENZHOU_MAHJONG_GAME_ID			= 8,	//温州麻将
	E_HORSE_GAME_ID						= 9,	//小马快跑
	E_FANFANLE_GAME_ID					= 10,	//翻翻乐
    E_FISH_GAME_ID						= 11,	//鱼
	E_FRYBULL_GAME_ID					= 12,	//炸金牛
	E_COMPALL_BULL_GAME_ID				= 13,	//通比牛牛
	E_TIGER_GAME_ID						= 14,	//虎虎生威
	E_MAAJAN_GAME_ID					= 15,   //双人麻将
	E_SICHUAN_MAHJONG_GAME_ID			= 16,	//四川麻将（血战到底）
	E_BACCARAT_GAME_ID					= 17,	//百家乐
	E_PAIGOW_GAME_ID					= 18,	//牌九
	E_BIRD_ANIMAL_GAME_ID				= 19,	//飞禽走兽
	E_BENZ_BMW_GAME_ID					= 20,	//奔驰宝马
	E_HEBEI_MAHJONG_GAME_ID				= 21,	//河北麻将
	E_GUOBIAO_MAHJONG_GAME_ID			= 22,   //国标麻将
	E_HUNAN_MAHJONG_GAME_ID				= 23,   //湖南麻将
	E_WUHAN_MAHJONG_KAIKOU_GAME_ID		= 24,   //武汉麻将 ,开口番
	E_WUHAN_MAHJONG_KOUKOU_GAME_ID		= 25,	//武汉麻将 ,口口番
	E_GUANGDONG_MAHJONG_GAME_ID			= 26,   //广东麻将
	E_ZHENGZHOU_MAHJONG_GAME_ID			= 27,	//郑州麻将
	E_TIGER_MACHINE_GAME_ID				= 28,	//老虎机
	E_BLACKJACK_GAME_ID					= 29,	//21点
	E_SHUANGKOU_GAME_ID					= 30,	//双扣
	E_BULL_2017							= 31,	//牛牛2017
	E_FLOWER_2017						= 32,	//金花2017
	E_LAND_2017							= 33,   //斗地主2017
	E_LAND_CARD_GAME_ID					= 34,   //斗地主房卡模式

	E_FISHPARTY_GAME_ID					= 50,	//捕鱼派对游戏

    ///////////////////////////////////////////////////
    E_HALL_SERVER_ID           = 10000, //大厅服务器
    E_USER_SERVER_ID           = 10001, //用户状态服务器
    E_MONEY_SERVER_ID          = 10002, //金币服务器
    E_MYSQL_SERVER_ID          = 10003, //数据库服务器
    E_TRUMPT_SERVER_ID         = 10004, //通知服务器
    E_ROUND_SERVER_ID          = 10005, //回合服务器
};

enum E_GAME_LEVEL_ID {
	E_PRIMARY_LEVEL = 1,			//0.1
	E_MIDDLE_LEVEL = 2,				//1
	E_ADVANCED_LEVEL = 3,			//5
	E_MASTER_LEVEL = 4,		    	//10
	E_LEVEL_HUNDRED = 5,        	//100
	E_LEVEL_FRIEND_PK = 6,			//
	E_LEVEL_PRIVATE = 7,        	//
	E_LEVEL_EXPERIENCE = 8,        	//
	E_LEVEL_PASSPORT = 9,			//房卡模式
};

//客户端平台类型
enum E_MSG_SOURCE_TYPE {
    E_MSG_SOURCE_UNKONW     = 0,
	E_MSG_SOURCE_ANDROID    = 1,
	E_MSG_SOURCE_IPHONE     = 2,
	E_MSG_SOURCE_IPAD       = 3,
    E_MSG_SOURCE_WEBSERVER  = 4,
	E_MSG_SOURCE_SERVER     = 5,    //unknown
	E_MSG_SOURCE_ROBOT		= 30,    //机器人
	//用于区分错误码
	E_MSG_SOURCE_ALLOC_SERVER 		   = 98,
	E_MSG_SOURCE_GAME_SERVER 		   = 99,
	E_MSG_SOURCE_HALL_SERVER           = 100, //大厅服务器
	E_MSG_SOURCE_USER_SERVERD          = 101, //用户状态服务器
	E_MSG_SOURCE_MONEY_SERVER          = 102, //金币服务器
	E_MSG_SOURCE_MYSQL_SERVER          = 103, //数据库服务器
	E_MSG_SOURCE_TRUMPT_SERVER         = 104, //通知服务器
};

//消息区间
enum E_MSG_ID_RANGE
{
	E_SERVER_PUBLIC_ID_START  = 0x0500,		//服务器之间公用消息
	E_SERVER_PUBLIC_ID_END    = 0x05FF,

	E_SERVER_PRIVATE_ID_START = 0x0600,		//服务器的私有消息
	E_SERVER_PRIVATE_ID_END   = 0x08FF,

	E_CLIENT_PUBLIC_ID_START  = 0x0900,		//客户端之间公用消息
	E_CLIENT_PUBLIC_ID_END    = 0x09FF,		//

	E_CLIENT_GAME_ID_START    = 0x0A00,     //客户端私有消息
	E_CLIENT_GAME_ID_END      = 0xFFFF,
};

//fucking old msg id.
enum E_MGS_ID_OLD {
    //HALL_USER_LOGIN	          = 0x0001,    // 用户注册信息
    //HALL_USER_LOGOUT		  = 0x0002,    // 用户注消信息
    // -5   已经在另外的游戏中
	// -11  已经在另外的等级房间中
	// 从userserver获取状态, 参考S2S_USER_GET_USER_STATUS
	//HALL_USER_ALLOC           = 0x0003,    // 用户申请桌子
    //HALL_QUICK_MATCH		  = 0x0004,    // 快速匹配
    //HALL_MSG_CHANGETAB		  = 0x0006,    // 换桌
    

    SERVER_HEART_BEAT		 = 0x0150,	   //服务器心跳
	HEART_BEAT			     = 0x0001,     //心跳
	//SERVER_REGIST			 = 0x0010,     //服务器注册

    //SERVER_CMD_KICK_OUT      = 0x0030,     //服务器主动踢人
    
    //PHP_CMD_RECHARGE		 = 0x0090,      //充值成功
    //PHP_CMD_FEEDBACK_PUSH	 = 0x0091,      //反馈推送
	
	//SERVER_CMD_TRUMPT		 = 0x0071,
	//CLIENT_CMD_STRUMPT		 = 0x0072,
	//SYSTEM_CMD_TRUMPET	     = 0x0073,
    
    //建议使用S2S_USER_NOTIFY_LEAVE
    //GMSERVER_LEAVE           = 0x0057,
    //建议使用S2S_USER_NOTIFY_ENTER
    //GMSERVER_ENTER           = 0x0059,
    //建议使用S2S_USER_NOTIFY_STATUS
    //GMSERVER_USER_STATUS     = 0x005A,
};

#endif //End for __PROTOCOL_H_

#ifndef __PROTOCOL_SERVER_ID_H_
#define __PROTOCOL_SERVER_ID_H_

#include "Protocol.h"

//喇叭发送类型
enum TrumpetType
{
    GAME_TRUMPET    = 5,		//在同一个产品的同一个游戏中发送
    PRODUCT_TRUMPET = 100		//在同一个产品的所有的游戏中发送
};

enum Server2ServerMsgID {
    //================================================================================
    //服务器直接公有协议 S2S_PUBLIC_
    //================================================================================
    // int svr id
    S2S_PUBLIC_HEARTBEAT_ID    = E_SERVER_PUBLIC_ID_START, //(0x0500)
    // server ==> server
	// string svr ip
	// int svr port
	// int svr id
	// byte game level  (default 0)
	// int game id      (E_GAMESERVER_ID)
	// int svr weight   (default 0)
    S2S_PUBLIC_REGISTER_ID     = 0x0501,
    
    S2S_PHP_NOTIFY_RELOAD_CONF = 0x04FF,  // php通知加载coin config
    S2S_PHP_NOTIFY_RECHARGE = 0x0090,  //php通知充值成功
	S2S_MONEY_NOTIFY_UPDATE_MONEY = 0x0420, //金币服务器更新金币通知游戏服务器
	
	S2S_PUBLIC_ID_END = E_SERVER_PUBLIC_ID_END,   //(0x05FF)

    //================================================================================
    //小喇叭服务器提供消息 S2S_TRUMPT_
    //================================================================================
    S2S_PRIVATE_ID_START     = E_SERVER_PRIVATE_ID_START,
    S2S_TRUMPT_GAMESERVER_ID = 0x0601,   //游戏服务器推送的喇叭消息
    S2S_TRUMPT_GAMECLINET_ID = 0x0602,   //游戏客户端推送的喇叭消息
    S2S_TRUMPT_SYSADMIN_ID   = 0x0603,   //后台管理推送的喇叭消息
	
	
    //================================================================================
    //大厅服务器提供消息 S2S_HALL_
    //================================================================================
    // int svr id
	// byte 是否开启限制 1 - 开启 0 - 关闭
	S2C_HALL_GAME_NOTFY_LIMIT  = 0x0610,	 // 通知大厅game负载到达限制


    //================================================================================
    //用户服务器提供消息  S2S_USER_
    //================================================================================
    // int   uid
    // short svid
    // short level
    S2S_USER_NOTIFY_LEAVE     = 0x0620,   //游戏服务器通知用户离开(0x0250的时候通知)
    // int   uid
    // short svid
    // short level
    // int   tid
	S2S_USER_NOTIFY_ENTER = 0x0621,   //游戏服务器通知用户进入(0x0200的时候通知)
	// int   uid
	// S2S_USER_NOTIFY_STATUS    = 0x0622,   //游戏服务器上报用户当前状态(0x0201的时候通知), 废弃
	//int count_users;
	//for (int i = 0; i < count_user; ++i)
	//{  int uid;
	//   short pid;
	//}
	S2S_USER_SYNC_HALL_STATUS = 0x0623,   //大厅服务器注册成功之后，同步当前在线玩家到UserServer
	//int count_table;
	//int count_user;
	//for (int i = 0; i < count_user; i++) {
	//    int userid;
	//    int tid;
	//}
	S2S_USER_SYNC_GAME_STATUS = 0x0624,    //游戏服务器注册成功之后，同步当前在线玩具到UserServer
	//request  : 
	//      int uid
	//response :
	//      int uid      //用户id
	//      int pid      //产品id
	//      int hallsvid //大厅服务器id
	//      int gametype //所在游戏类型
	//      short level    //游戏等级
	//      int gamesvid //游戏服务器id 
	//      int tid      //桌子id 
	S2S_USER_GET_USER_STATUS = 0x0625,    //获取用户状态
	//request:  byte change (1 = add, 0 = delete)
	//          int tidcount
	S2S_USER_NOTIFY_CHANGE_TABLE = 0x0626, //游戏桌子数量变更    

	//================================================================================
	//经验服务器提供消息  S2S_ROUND_
	//================================================================================
	/// round->game
	// int uid
	// short table index
	// int win
	// int lose
	// int tie
	// int runaway
	// int64 max win
	// int64 max value
	/// game->round
	// int uid
	// short table index
	S2S_ROUND_GET_INFO = 0x0630,
	/// game->round
	// int uid
	// int win
	// int lose
	// int tie
	// int runaway
	S2S_ROUND_SET_INFO = 0x0631,
	/// game->round
	// int uid
	// int online time
	S2S_ROUND_SET_ONLINETIME = 0x0632,

	//================================================================================
	//mysql服务器提供消息  S2S_MYSQL_
	//================================================================================
	/// game->mysql
	//string
	/*
	 * json format
	 * {
	 *		"uid":[uid1,uid2,...],
	 *		"winlose":[100,-100,...],
	 *		"gameid":3,
	 *		"level":1,
	 *		"tid":1,
	 *		"end_time":12344354324,
	 *		"data":""
	 * }
	 */
	S2S_MYSQL_RECORD_GAME_DATA = 0x0640,
	//================================================================================
	//mysql服务器提供消息  S2S_MYSQL_
	//================================================================================
	/// game->mysql
	//string
	/*
	 * json format
	 * {
	 *		"mid":uid,
	 *		"type":1  // 1 login; 0 logout
	 *		"ip":ip,
	 *		"addr":1,
	 *		"pid":1,
	 *		"ctime":12344354324,
	 * }
	 */
	S2S_MYSQL_LOG_LOGIN_DATA = 0x0641,

	/// game->mysql
	// int owner
	// int gameid
	// int tid
	// int max play count
	// string player info, json
	// int create time
	// int add or update, 1 -- add, 0 -- update
	S2S_MYSQL_ADD_CARDROOM_HISTORY = 0x0642,
	// hall->mysql
	//
	S2S_MYSQL_ADD_CARDROOM_RECORD = 0x0643,

	//================================================================================
	//服务器管理消息  S2S_ADMIN_
	//================================================================================
	S2S_ADMIN_ID_BEGIN			= 0x0660,
	S2S_ADMIN_GET_ALL_PLAYER	= S2S_ADMIN_ID_BEGIN,
	S2S_ADMIN_GET_FREE_PLAYER	= 0x0661,
	S2S_ADMIN_GET_IDLE_TABLE	= 0x0662,
	S2S_ADMIN_GET_ROOMINFO		= 0x0663,
	S2S_ADMIN_GET_SYS_CFG		= 0x0664,
	S2S_ADMIN_PHP_PUSH_RELOAD	= S2S_PHP_NOTIFY_RELOAD_CONF,	//TODO
	S2S_ADMIN_ID_END			= 0x0680,
    
    S2S_PRIVATE_ID_END      = E_SERVER_PRIVATE_ID_END,
};

// 小喇叭以及user，money均有个gameServerMap数组，以gameid作为索引，因此可能导致越界，暂时将
// MAX_GAME统一到这里
#define MAX_GAME 128

//玩家之间输赢记录
const int S2S_MYSQL_LOG_WIN_LOSE = 0x0121;

const int S2S_MYSQL_PLAYING_INFO = 0x0122;

const int S2S_HALL_REGIST_ALLOC_ID       = 0x0009;    //
const int S2S_HALL_REGIST_GAME_ID        = 0x0020;
const int S2S_HALL_HEART_BEAT_ID         = 0x0150;

const int ADMIN_CMD_RELOAD               = 0x0303;
////////////////////////////////////////////////////////////////////////////////////
//  Mysql Server Command ID
////////////////////////////////////////////////////////////////////////////////////
const int CMD_MYSQLSVR_START_ID          = 0x0500;
const int CMD_MYSQLSVR_HUNDRED_DISCOUNT  = 0x0501; //保存百人场暗扣记录

const int CMD_MYSQLSVR_END_ID            = 0x05FF;

const int CMD_MONEY_UPDATE_MONEY		  = 0x0462;//更新金币信息
const int CMD_MONEY_UPDATE_COIN		  = 0x0464;//更新房卡信息
const int CMD_UPDATE_EXP = 0x0467;//更新乐币
const int CMD_UPDATE_SAFEMONEY = 0x0463;//更新保险箱信息
const int CMD_UPDATE_LEAVE_GAME_SERVER = 0x0468;//用户离开游戏服务器

////////////////////////////////////////////////////////////////////////////////////
// common msg id, old
const int HALL_REGIST_INFO = 0x0020;	//大厅注册
const int HALL_USER_LOGOUT = 0x0002;                // 用户注消信息
const int SERVER_REGIST = 0x0010;

//=================系统管理接口=============================//
const int ADMIN_MSG_ROOMINFO = 0x0080;         //取房间信息
const int ADMIN_GET_FREE_PLAYER = 0x0081;         //取闲置用户
const int ADMIN_GET_SYS_CFG = 0x0082;         //取系统当前配置
const int ADMIN_RELOAD = 0x0083;         //重新加载管理数据
const int ADMIN_GET_ALL_PLAYER = 0x0084;         //取所有用户信息
const int ADMIN_GET_IDLE_TABLE = 0x0085;         //获取空闲桌子
//================游戏服务器上报给ALLOC=======================//
const int GMSERVER_LEAVE = 0x0057;        //游戏服务器通知用户离开
const int GMSERVER_ENTER = 0x0059;        //游戏服务器通知用户进入
const int GMSERVER_USER_STATUS = 0x005A;     //游戏服务器上报用户当前状态
const int GMSERVER_SET_TABLEUSER = 0x005B;		//游戏服务器更新桌子人数
const int GMSERVER_SET_TABLE_STATUS = 0x005C;		//游戏服务器更新桌子状态
const int GMSERVER_APPLY_MATCH = 0x005E;  	//重新匹配

const int ROUND_GET_INFO = 0x0480;//获取用户信息，局数和经验等
const int ROUND_SET_INFO = 0x0481;//设置用户信息，局数和经验等
const int ROUND_SET_ONLINETIME = 0x0482;//设置在线时长

const int GMSERVER_REPORT     = 0x0051;        //游戏服务器上报
const int GMSERVER_UPDATE     = 0x0052;        //游戏服务器更新人数
const int GMSERVER_SET_STATUS = 0x0053;		//设置游戏服务器更新状态

const int GMSERVER_CMD_TRUMPT			= 0x0071; //游戏服务器发送喇叭

#endif // End for __PROTOCOL_SERVER_ID_H_


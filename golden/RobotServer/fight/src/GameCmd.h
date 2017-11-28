#ifndef GAMECMD_H
#define GAMECMD_H

const int ADMIN_GET_FREE_PLAYER = 0x0081;         //取闲置用户
const int ADMIN_GET_IDLE_TABLE  = 0x0085;         //获取空闲桌子

const int SERVER_GAMESERVER_CLOSED		= 0x0301; //游戏服务器异常关闭
const int ALLOC_TRAN_GAMEINFO			= 0x0302; //匹配服务器传送GameServer信息给大厅，方便大厅连接

const int HALL_REGIST					= 0x0009;                  // 大厅注册信息
const int HALL_USER_LOGIN	            = 0x0001;                  // 用户注册信息
const int HALL_USER_LOGOUT				= 0x0002;                // 用户注消信息
const int HALL_USER_ALLOC				= 0x0003 ;                 // 用户申请桌子
const int HALL_QUICK_MATCH				= 0x0004 ;                 // 快速匹配
const int HALL_GET_MEMBERS				= 0x0005;                  // 取各个游戏场用户数
const int HALL_MSG_CHANGETAB			= 0x0006;                  // 换桌
const int SERVER_CMD_KICK_OUT			= 0x0030; //踢出用户
const int HALL_HEART_BEAT               = 0x0150;  //大厅给匹配服务器和游戏服务器发送的心跳包
const int HALL_REGIST_INFO				= 0x0020;	//大厅注册

const int SERVER_MSG_ROBOT				= 0x0244; //给机器人发送详细信息

const int ADMIN_CMD_RELOAD                              = 0x0300;

//================客户端命令=======================//
const int USER_HEART_BEAT			    = 0x0101;  //用户心跳，设置过期时间
const int CLIENT_MSG_LOGINCOMING		= 0x0200; //用户登录并且进入房间
const int CLIENT_MSG_START_GAME 		= 0x0201;  //用户准备
const int CLIENT_MSG_BET_CALL			= 0x0203; //跟注
const int CLIENT_MSG_BET_RASE			= 0x0204; //加注
const int CLIENT_MSG_ALL_IN				= 0x0205; //ALLIN
const int CLIENT_MSG_CHECK_CARD			= 0x0206; //看牌
const int CLIENT_MSG_THROW_CARD			= 0x0207; //弃牌
const int CLIENT_MSG_COMPARE_CARD		= 0x0208; //比牌
const int GMSERVER_MSG_GAMEOVER			= 0x0209; //游戏结束
const int GMSERVER_GAME_START			= 0x020A; //游戏服务器推送游戏开始
const int GMSERVER_BET_TIMEOUT			= 0x020B; //游戏服务器推送下注超时
const int CLIENT_MSG_CHAT				= 0x020E; //聊天
const int GMSERVER_WARN_KICK			= 0x020D; //游戏服务器通知用户快要踢出他了
const int PUSH_MSG_FORBID				= 0x0210; //推送再玩用户禁比轮数
const int CLIENT_MSG_FORBIDDEN          = 0x0212; //禁比
const int CLIENT_MSG_LEAVE              = 0x0250; //用户离开
const int CLIENT_MSG_TABLEDET			= 0x0251; //用户获取牌局详细信息
const int CLIENT_MSG_LOGINCHANGE		= 0x0252; //登录并进入换桌房间
const int SERVER_MSG_KICKOUT			= 0x0253; //游戏服务器踢出用户命令
const int CLIENT_MSG_AWAY				= 0x020C; //用户离开

const int EXIT			= -9999; 

#endif

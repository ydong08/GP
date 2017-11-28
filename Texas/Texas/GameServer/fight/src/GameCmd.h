#ifndef GAMECMD_H
#define GAMECMD_H

const int PRIVATE_ROOM				= 25;
const int LEVEL1					= 6;
const int LEVEL2					= 12;
const int LEVEL3					= 21;
const int LEVEL6					= 27;

const int HALL_HEART_BEAT               = 0x0150;  //大厅给匹配服务器和游戏服务器发送的心跳包

//=================私人房=============================//
const int CLIENT_CREATE_PRIVATE			= 0x0011;
const int CLIENT_GET_PRIVATELIST		= 0x0012;
const int CLIENT_ENTER_PRIVATE			= 0x0013;


//================游戏服务器上报给ALLOC=======================//
const int REPORT_ROOM_INFO				= 0x0061; //上报房间信息
const int REPORT_SERVER_PORTIP			= 0x0064; //上报server的IP Port

const int SERVER_MSG_ROBOT				= 0x0244; //给机器人发送详细信息

//================客户端命令=======================//
//const int USER_HEART_BEAT			    = 0x0101;  //用户心跳，设置过期时间
const int CLIENT_MSG_LOGINCOMING		= 0x0200; //用户登录并且进入房间
const int CLIENT_MSG_START_GAME 		= 0x0201;  //用户准备
const int GMSERVER_MSG_SEND_CARD        = 0x0202; //游戏开始推送，游戏开始，发牌
const int CLIENT_MSG_BET_CALL			= 0x0203; //跟注
const int CLIENT_MSG_BET_RASE			= 0x0204; //加注
const int CLIENT_MSG_ALL_IN				= 0x0205; //ALLIN
const int CLIENT_MSG_CHECK_CARD			= 0x0206; //看牌
const int CLIENT_MSG_THROW_CARD			= 0x0207; //弃牌
const int GMSERVER_MSG_GAMEOVER			= 0x0208; //游戏结束
const int GMSERVER_GAME_START			= 0x0209; //游戏服务器推送游戏开始
const int GMSERVER_BET_TIMEOUT			= 0x020A; //游戏服务器推送下注超时
const int CLIENT_MSG_CHAT				= 0x022C; //聊天
const int CLIENT_MSG_SEND_GIFT			= 0x023F; //送礼物
const int CLIENT_GET_POOL				= 0x020E; //推送底池数量
const int CLIENT_SET_CARRY				= 0x020F; //重新买入金币
const int GMSERVER_WARN_KICK			= 0x020D; //游戏服务器通知用户快要踢出他了
const int PUSH_PRIVATE_OP				= 0x0210; //私人场推送用户只有看牌和弃牌选项
const int CLIENT_MSG_TAKESEAT   		= 0x0213; //离开或就座
const int CLIENT_MSG_LEAVE              = 0x0250; //用户离开
const int CLIENT_MSG_TABLEDET			= 0x0251; //用户获取牌局详细信息
const int CLIENT_MSG_LOGINCHANGE		= 0x0252; //登录并进入换桌房间
const int SERVER_MSG_KICKOUT			= 0x0253; //游戏服务器踢出用户命令
const int CLIENT_MSG_LOGOUT             = 0x020C; //用户离开

const int UPDATE_MONEY					= 0x0462;//更新金币信息
const int UPDATE_LEAVE_GAME_SERVER		= 0x0468;//用户离开游戏服务器

const int MONEYSERVER_UPDATE_USERCOIN   = 0x0420;//金币服务器更新金币通知游戏服务器

#endif

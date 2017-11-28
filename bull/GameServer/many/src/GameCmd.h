#ifndef GAMECMD_H
#define GAMECMD_H

//=================系统管理接口=============================//
const int ADMIN_GET_PLAY_INFO   = 0x0085;         //获取人数信息

//=================私人房=============================//
const int CLIENT_CREATE_PRIVATE			= 0x0011;
const int CLIENT_GET_PRIVATELIST		= 0x0012;
const int CLIENT_ENTER_PRIVATE			= 0x0013;

//================游戏服务器上报给ALLOC=======================//
const int GMSERVER_ALL		  =	0x0054;		//管理端获取所有游戏服务器负载信息
const int GMSERVER_GET_INFO   = 0x0055;		//管理端获取指定游戏服务器负载信息
const int GMSERVER_SET_INFO	  = 0x0056;		//管理端设置指定游戏服务器配置信息
const int REPORT_ROOM_INFO				= 0x0061; //上报房间信息
const int REPORT_SERVER_PORTIP			= 0x0064; //上报server的IP Port

const int SERVER_MSG_ROBOT				= 0x0244; //给机器人发送详细信息

const int CLIENT_MSG_LOGINCOMING		= 0x0200; //用户登录并且进入房间
const int CLIENT_MSG_START_GAME 		= 0x0201;  //用户准备
const int GMSERVER_GAME_START			= 0x0202; //游戏开始发前面三张牌
const int CLIENT_MSG_CALL_BANKER        = 0x0203; //抢庄
const int GMSERVER_GAME_CONFIRM			= 0x0204; //确定荘家推送
const int CLIENT_MSG_SET_NUL			= 0x0205; //玩家翻倍
const int GMSERVER_MSG_OPEN				= 0x0206; //通知玩家要开牌了
const int CLIENT_MSG_OPEN_CARD			= 0x0207; //玩家开牌
const int CLIENT_MSG_CHAT				= 0x0209; //用户聊天
const int CLIENT_MSG_HELP				= 0x020A; //提醒

const int GMSERVER_WARN_KICK			= 0x020D; //游戏服务器通知用户快要踢出他

//====================百人牛牛==========================

const int GMSERVER_MSG_TABSTATUS		= 0x0210; //更新桌子状态
const int GMSERVER_MSG_NOTBETINFO		= 0x0211; //推送下注信息
const int GMSERVER_MSG_NOTELEAVE		= 0x0212; //推送荘家离开
const int GMSERVER_MSG_ROTATEBANKE		= 0x0213; //轮换荘家
const int CLIENT_MSG_APPLYBANKER		= 0x0214; //申请上庄
const int CLIENT_MSG_CANCLEBANKER		= 0x0215; //申请下庄
const int CLIENT_MSG_BET_COIN			= 0x0216; //下注
const int CLIENT_MSG_GETBANKERLIST		= 0x0217; //获取荘家列表
const int CLIENT_MSG_GETHISTORY			= 0x0218; //获取路数
const int GMSERVER_MSG_GAMEOVER			= 0x0219; //游戏结束
const int CLIENT_MSG_GETPLAYERRLIST     = 0x021A; //
const int SERVER_MSG_UPDATE_MOENY		= 0x021B; //
const int CLIENT_MSG_APPLY_SEAT         = 0x022E; //申请座位
const int CLIENT_MSG_REFRESH_SEATLIST   = 0x022F; //获取座位列表

// short retcode
// string ret msg
// int  notify uid
// int  vip uid
// for i = 1;i < 5
// short vip continue count
// int64 vip bet money
// end for
// int banker uid
// int64 banker money
const int GMSREVER_MSG_NOTIFY_ROBOT		= 0x0230; //仅用于通知机器人玩家下注情况超过预设条件

//======================================================
const int CLIENT_MSG_LEAVE              = 0x0250; //用户离开
const int CLIENT_MSG_TABLEDET			= 0x0251; //用户获取牌局详细信息
const int CLIENT_MSG_LOGINCHANGE		= 0x0252; //登录并进入换桌房间
const int SERVER_MSG_KICKOUT			= 0x0253; //游戏服务器踢出用户命令
const int CLIENT_MSG_LOGOUT             = 0x020C; //用户离开

const int UPDATE_MONEY					= 0x0462;//更新金币信息
const int UPDATE_LEAVE_GAME_SERVER		= 0x0468;//用户离开游戏服务器

const int MONEYSERVER_UPDATE_USERCOIN   = 0x0420;//金币服务器更新金币通知游戏服务器

// int   庄家牌型
// int   天位牌型
// int   地位牌型
// int   玄位牌型
// int   黄位牌型
// 牌型
// 0 无牛
// 1 牛1
// 2 牛2
// 3 牛3
// ……
// 9 牛9
// 10 牛牛
const int PHP_PUSH_CARD					= 0x0501; //php服务器推送本轮卡牌

#endif

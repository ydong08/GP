#ifndef GAMECMD_H
#define GAMECMD_H

const int HALL_HEART_BEAT               = 0x0150;  //大厅给匹配服务器和游戏服务器发送的心跳包

//=================私人房=============================//
const int CLIENT_CREATE_PRIVATE			= 0x0011;
const int CLIENT_GET_PRIVATELIST		= 0x0012;
const int CLIENT_ENTER_PRIVATE			= 0x0013;

const int ADMIN_GET_PLAY_INFO   = 0x0085;         //获取人数信息

//================游戏服务器上报给ALLOC=======================//
const int GMSERVER_ALL		  =	0x0054;		//管理端获取所有游戏服务器负载信息
const int GMSERVER_GET_INFO   = 0x0055;		//管理端获取指定游戏服务器负载信息
const int GMSERVER_SET_INFO	  = 0x0056;		//管理端设置指定游戏服务器配置信息
const int REPORT_ROOM_INFO				= 0x0061; //上报房间信息
const int REPORT_SERVER_PORTIP			= 0x0064; //上报server的IP Port

const int SERVER_MSG_ROBOT				= 0x0244; //给机器人发送详细信息

const int TRANS_GAME_SERVER_START		= 0x0200;

const int CLIENT_MSG_LOGINCOMING		= 0x0200; //用户登录并且进入房间
const int CLIENT_MSG_START_GAME 		= 0x0201;  //用户准备


//====================百人德州==========================

const int GMSERVER_MSG_TABSTATUS		= 0x0240; //更新桌子状态
const int GMSERVER_MSG_NOTBETINFO		= 0x0241; //推送下注信息
const int GMSERVER_MSG_NOTELEAVE		= 0x0242; //推送荘家离开
const int GMSERVER_MSG_ROTATEBANKE		= 0x0243; //轮换荘家
const int CLIENT_MSG_APPLYBANKER		= 0x0244; //申请上庄
const int CLIENT_MSG_CANCLEBANKER		= 0x0245; //申请下庄
const int CLIENT_MSG_BET_COIN			= 0x0246; //下注
const int CLIENT_MSG_GETBANKERLIST		= 0x0247; //获取荘家列表
const int CLIENT_MSG_GETHISTORY			= 0x0248; //获取路数
const int GMSERVER_MSG_GAMEOVER			= 0x0249; //游戏结束
const int CLIENT_MSG_GETPLAYERRLIST     = 0x024A; //
const int SERVER_MSG_UPDATE_MOENY		= 0x024B; //
const int CLIENT_MSG_CHAT               = 0x024C; //用户聊天
const int CLIENT_MSG_GETJACKPOT         = 0x024D; //获取奖池信息
const int CLIENT_MSG_APPLY_SEAT			= 0x022E; //申请座位
const int CLIENT_MSG_REFRESH_SEATLIST	= 0x022F; //获取座位列表

//======================================================
const int CLIENT_MSG_LEAVE              = 0x0250; //用户离开
const int CLIENT_MSG_TABLEDET			= 0x0251; //用户获取牌局详细信息
const int CLIENT_MSG_LOGINCHANGE		= 0x0252; //登录并进入换桌房间
const int SERVER_MSG_KICKOUT			= 0x0253; //游戏服务器踢出用户命令
const int CLIENT_MSG_LOGOUT             = 0x020C; //用户离开

//const int CLIENT_MSG_GET_MONEY = 0x0260; //客户端请求获取金币

const int UPDATE_MONEY					= 0x0462;//更新金币信息
const int UPDATE_SAFEMONEY				= 0x0463;//更新保险箱信息
const int UPDATE_ROLL					= 0x0464;//更新乐劵
const int UPDATE_ROLL1					= 0x0465;//更新过期乐劵
const int UPDATE_COIN					= 0x0466;//更新乐币
const int UPDATE_EXP					= 0x0467;//更新乐币
const int UPDATE_LEAVE_GAME_SERVER		= 0x0468;//用户离开游戏服务器

const int MONEYSERVER_UPDATE_USERCOIN   = 0x0420;//金币服务器更新金币通知游戏服务器

// int   庄家输赢
// int   天位输赢
// int   地位输赢
// int   玄位输赢
// int   黄位输赢
// 2 赢  0 输 
// 1 仅庄家使用
// 如果庄家为1，那么就分别看天地玄黄的数值
// 1 2 0 2 0
// 天 > 庄
// 地 < 庄
// 玄 > 庄
// 黄 < 庄
const int PHP_PUSH_CARD = 0x0501; //php服务器推送本轮卡牌

#endif

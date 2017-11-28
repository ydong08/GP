#ifndef GAMECMD_H
#define GAMECMD_H

//=================系统管理接口=============================//
const int ADMIN_GET_PLAY_INFO = 0x0085;         //获取人数信息

												//=================私人房=============================//
const int CLIENT_CREATE_PRIVATE = 0x0011;
const int CLIENT_GET_PRIVATELIST = 0x0012;
const int CLIENT_ENTER_PRIVATE = 0x0013;

//================游戏服务器上报给ALLOC=======================//
const int GMSERVER_ALL = 0x0054;		//管理端获取所有游戏服务器负载信息
const int GMSERVER_GET_INFO = 0x0055;		//管理端获取指定游戏服务器负载信息
const int GMSERVER_SET_INFO = 0x0056;		//管理端设置指定游戏服务器配置信息
const int REPORT_ROOM_INFO = 0x0061; //上报房间信息
const int REPORT_SERVER_PORTIP = 0x0064; //上报server的IP Port

const int SERVER_MSG_ROBOT = 0x0244; //给机器人发送详细信息

const int CLIENT_MSG_LOGINCOMING = 0x0200; //用户登录并且进入房间
const int CLIENT_MSG_START_GAME = 0x0201;  //用户准备
const int GMSERVER_GAME_START = 0x0202; //游戏开始发前面三张牌
const int CLIENT_MSG_CALL_BANKER = 0x0203; //抢庄
const int GMSERVER_GAME_CONFIRM = 0x0204; //确定荘家推送
const int CLIENT_MSG_SET_NUL = 0x0205; //玩家翻倍
const int GMSERVER_MSG_OPEN = 0x0206; //通知玩家要开牌了
const int CLIENT_MSG_OPEN_CARD = 0x0207; //玩家开牌
//const int CLIENT_MSG_CHAT = 0x0209; //用户聊天
const int CLIENT_MSG_HELP = 0x020A; //提醒

const int GMSERVER_WARN_KICK = 0x020D; //游戏服务器通知用户快要踢出他
//====================龙虎斗==========================

const int GMSERVER_MSG_TABSTATUS		= 0x0220; //更新桌子状态
const int GMSERVER_MSG_NOTBETINFO		= 0x0221; //推送下注信息
const int GMSERVER_MSG_NOTELEAVE		= 0x0222; //推送荘家离开
const int GMSERVER_MSG_ROTATEBANKE		= 0x0223; //轮换荘家
const int CLIENT_MSG_APPLYBANKER		= 0x0224; //申请上庄
const int CLIENT_MSG_CANCLEBANKER		= 0x0225; //申请下庄
const int CLIENT_MSG_BET_COIN			= 0x0226; //下注
const int CLIENT_MSG_GETBANKERLIST		= 0x0227; //获取荘家列表
const int CLIENT_MSG_GETHISTORY			= 0x0228; //获取路数
const int GMSERVER_MSG_GAMEOVER			= 0x0229; //游戏结束
const int CLIENT_MSG_GETPLAYERRLIST     = 0x022A; //
const int SERVER_MSG_UPDATE_MOENY		= 0x022B; //
const int CLIENT_MSG_CHAT               = 0x022C; //用户聊天
const int CLIENT_MSG_APPLY_SEAT         = 0x022E; //申请座位
const int CLIENT_MSG_REFRESH_SEATLIST   = 0x022F; //获取座位列表
const int SERVER_NOTIFY_GAMERESULT		= 0x0240; //广播给客户端所有玩家的结算信息
const int CLIENT_MSG_LOGINCHANGE		= 0x0241; //登录并进入换桌房间

// request:
//		int uid
//		int tid
//		byte action (0表示离座，1表示就座，2表示换座，3庄家申请离庄)
//		byte seat id
// response (scene broadcast):
//		short retcode
//		string retstring
//		int dest id
//		short dest status
//		int tid
//		short table status
//		short requestor status
//		int requestor id
//		string requestor name
//		int64 requestor money
//		string jsonStr
//		byte action
//		byte seat id
const int CLIENT_MSG_STANDUP = 0x0213; // 请求站起

// c -> s
// int uid
// int tid
// short seat id
// short emotion id
// s -> c
// short retcode
// string retstring
// int   target user id
// int   send emotion user id
// short seat id
// short emotion id
const int CLIENT_MSG_EMOTION            = 0x0230; //转发客户端聊天表情

//======================================================
const int CLIENT_MSG_LEAVE              = 0x0250; //用户离开
const int CLIENT_MSG_TABLEDET			= 0x0251; //用户获取牌局详细信息
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


// int 闲对
// int 闲
// int 和
// int 庄
// int 庄对
const int PHP_PUSH_CARD = 0x0501; //php服务器推送本轮卡牌

#endif

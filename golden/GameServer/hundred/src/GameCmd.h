#ifndef GAMECMD_H
#define GAMECMD_H

//const int HALL_HEART_BEAT               = 0x0150;  //大厅给匹配服务器和游戏服务器发送的心跳包
//const int HALL_REGIST_INFO				= 0x0020;	//大厅注册
//const int HALL_USER_LOGOUT           = 0x0002;                // 用户注消信息
//const int SERVER_REGIST					= 0x0010;

//=================私人房=============================//
const int CLIENT_CREATE_PRIVATE			= 0x0011;
const int CLIENT_GET_PRIVATELIST		= 0x0012;
const int CLIENT_ENTER_PRIVATE			= 0x0013;

//=================系统管理接口=============================//
//const int ADMIN_MSG_ROOMINFO	= 0x0080;         //取房间信息
//const int ADMIN_GET_FREE_PLAYER = 0x0081;         //取闲置用户
//const int ADMIN_GET_SYS_CFG     = 0x0082;         //取系统当前配置
//const int ADMIN_RELOAD          = 0x0083;         //重新加载管理数据
//const int ADMIN_GET_ALL_PLAYER  = 0x0084;         //取所有用户信息
//const int ADMIN_GET_IDLE_TABLE  = 0x0085;         //获取空闲桌子
const int ADMIN_GET_PLAY_INFO   = 0x0085;         //获取人数信息

//================游戏服务器上报给ALLOC=======================//
//const int GMSERVER_REPORT     = 0x0051;        //游戏服务器上报
//const int GMSERVER_UPDATE     = 0x0052;        //游戏服务器更新人数
//const int GMSERVER_SET_STATUS = 0x0053;		//设置游戏服务器更新状态
//const int GMSERVER_ALL		  =	0x0054;		//管理端获取所有游戏服务器负载信息
//const int GMSERVER_GET_INFO   = 0x0055;		//管理端获取指定游戏服务器负载信息
//const int GMSERVER_SET_INFO	  = 0x0056;		//管理端设置指定游戏服务器配置信息
//const int GMSERVER_LEAVE      = 0x0057;        //游戏服务器通知用户离开
//const int GMSERVER_ENTER      = 0x0059;        //游戏服务器通知用户进入
//const int GMSERVER_USER_STATUS   = 0x005A;     //游戏服务器上报用户当前状态
//const int GMSERVER_SET_TABLEUSER = 0x005B;		//游戏服务器更新桌子人数
//const int GMSERVER_SET_TABLE_STATUS = 0x005C;		//游戏服务器更新桌子状态
//const int REPORT_ROOM_INFO				= 0x0061; //上报房间信息
//const int REPORT_SERVER_PORTIP			= 0x0064; //上报server的IP Port
//
//const int SERVER_MSG_ROBOT				= 0x0224; //给机器人发送详细信息

//================客户端命令=======================//
//const int USER_HEART_BEAT			    = 0x0101;  //用户心跳，设置过期时间

const int TRANS_GAME_SERVER_START		= 0x0200;

const int CLIENT_MSG_LOGINCOMING		= 0x0200; //用户登录并且进入房间
const int CLIENT_MSG_START_GAME 		= 0x0201;  //用户准备


//====================百人金花==========================

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
const int CLIENT_MSG_GETJACKPOT         = 0x022D; //获取奖池信息
const int CLIENT_MSG_APPLY_SEAT         = 0x022E; //申请座位
const int CLIENT_MSG_REFRESH_SEATLIST   = 0x022F; //获取座位列表

//======================================================
const int CLIENT_MSG_LEAVE              = 0x0250; //用户离开
const int CLIENT_MSG_TABLEDET			= 0x0251; //用户获取牌局详细信息
const int CLIENT_MSG_LOGINCHANGE		= 0x0252; //登录并进入换桌房间
const int SERVER_MSG_KICKOUT			= 0x0253; //游戏服务器踢出用户命令
const int CLIENT_MSG_LOGOUT             = 0x020C; //用户离开

//const int CLIENT_MSG_GET_MONEY = 0x0260; //客户端请求获取金币

//const int GMSERVER_CMD_TRUMPT			= 0x0071; //游戏服务器发送喇叭

//enum TrumpetScope //喇叭发送的范围
//{
//	GAME_TRUMPET = 5,			//在同一个产品的同一个游戏中发送
//	PRODUCT_TRUMPET = 100		//在同一个产品的所有的游戏中发送
//};

const int UPDATE_MONEY					= 0x0462;//更新金币信息
const int UPDATE_SAFEMONEY				= 0x0463;//更新保险箱信息
const int UPDATE_ROLL					= 0x0464;//更新乐劵
const int UPDATE_ROLL1					= 0x0465;//更新过期乐劵
const int UPDATE_COIN					= 0x0466;//更新乐币
const int UPDATE_EXP					= 0x0467;//更新乐币
const int UPDATE_LEAVE_GAME_SERVER		= 0x0468;//用户离开游戏服务器

const int MONEYSERVER_UPDATE_USERCOIN   = 0x0420;//金币服务器更新金币通知游戏服务器

//const int ROUND_GET_INFO				= 0x0480;//获取用户信息，局数和经验等
//const int ROUND_SET_INFO				= 0x0481;//设置用户信息，局数和经验等
//const int ROUND_SET_ONLINETIME			= 0x0482;//设置在线时长
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

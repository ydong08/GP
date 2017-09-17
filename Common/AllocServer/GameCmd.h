#ifndef CMD_H
#define CMD_H

//这里的定义和etc/server.xml的配置nodes id 一一对应
const int USER_SERVER_ID_1 = 1;

const int ALLOC_REGIST					= 0x0010;	// ALLOC注册信息

const int ALLOC_TRAN_GAMEINFO		=	0x0302; //匹配服务器传送GameServer信息给大厅，方便大厅连接

//=======================大厅服务器===============================
const int HALL_REGIST					 = 0x0009;                  // 大厅注册信息
const int HALL_USER_LOGIN	            = 0x0001;                  // 用户注册信息
const int HALL_USER_LOGOUT           = 0x0002;                // 用户注消信息
const int HALL_USER_ALLOC              = 0x0003 ;                 // 用户申请桌子
const int HALL_QUICK_MATCH				= 0x0004 ;                 // 快速匹配
const int HALL_GET_MEMBERS             = 0x0005;                  // 取各个游戏场用户数
const int HALL_MSG_CHANGETAB			= 0x0006;                  // 换桌
const int HALL_HEART_BEAT               = 0x0150;  //大厅给匹配服务器和游戏服务器发送的心跳包
const int ALLOC_HEART_BEAT				= 0x0150;	  //ALLOC给用户服务器发送的心跳包
const int USER_HEART_BEAT			     = 0x0101;  //%%用户心跳，设置过期时间

const int SERVER_CMD_KICK_OUT			= 0x0030; //踢出用户

//=================私人房=============================//
const int CLIENT_CREATE_PRIVATE			= 0x0011;
const int CLIENT_GET_PRIVATELIST		= 0x0012;
const int CLIENT_ENTER_PRIVATE			= 0x0013;
const int CLIENT_FIND_PRIVATE			= 0x0014;


//================游戏服务器上报给ALLOC=======================//
const int GMSERVER_REPORT     = 0x0051;        //游戏服务器上报
const int GMSERVER_UPDATE     = 0x0052;        //游戏服务器更新人数
const int GMSERVER_SET_STATUS = 0x0053;		//设置游戏服务器更新状态
const int GMSERVER_ALL		  =	0x0054;		//管理端获取所有游戏服务器负载信息
const int GMSERVER_GET_INFO   = 0x0055;		//管理端获取指定游戏服务器负载信息
const int GMSERVER_SET_INFO	  = 0x0056;		//管理端设置指定游戏服务器配置信息
const int GMSERVER_LEAVE      = 0x0057;        //游戏服务器通知用户离开
const int GMSERVER_ENTER      = 0x0059;        //游戏服务器通知用户进入
const int GMSERVER_USER_STATUS   = 0x005A;     //游戏服务器上报用户当前状态
const int GMSERVER_SET_TABLEUSER = 0x005B;		//游戏服务器更新桌子人数
const int GMSERVER_SET_TABLE_STATUS = 0x005C;		//游戏服务器更新桌子状态
const int GMSERVER_BOOK_TABLE	    = 0x005D;  	//预定桌子 
const int GMSERVER_APPLY_MATCH	    = 0x005E;  	//重新匹配
const int REPORT_ROOM_INFO				= 0x0061; //上报房间信息
const int REPORT_SERVER_PORTIP			= 0x0064; //上报server的IP Port

const int START_ROBOT	    = 0x0044;  	//调取机器人 

const int USER_AGREE				= 0x004B;	//比赛服务器通知游戏服务器用户同意进入比赛

//============================管理接口===========================//
const int ADMIN_SET_STATUS     = 0x0083;      //管理端设置游戏服务器更新状态
const int ADMIN_GET_ALL        = 0x0084;      //管理端获取所有游戏服务器负载信息
const int ADMIN_GET_INFO   = 0x0085;      //管理端获取指定游戏服务器负载信息
const int ADMIN_SET_INFO   = 0x0086;      //管理端设置指定游戏服务器配置信息
const int ADMIN_RELOAD_MTIME			 = 0x0080; //重新加载匹配时间

const int ADMIN_CMD_RELOAD_ALLOC			= 0x0302; //重新load配置信息

//============================PHP充值成功推送===========================//
const int PHP_CMD_RECHARGE			= 0x0090; //充值成功
const int FEEDBACK_CMD_PUSH			= 0x0091; //反馈推送

//============================小喇叭===========================//
const int CMD_TRUMPT			= 0x0071; //喇叭
const int CLIENT_CMD_STRUMPT	= 0x0072; //发送喇叭

enum TrumpetScope //喇叭发送的范围
{
	GAME_TRUMPET = 5,			//在同一个产品的同一个游戏中发送
	PRODUCT_TRUMPET = 100		//在同一个产品的所有的游戏中发送
};

#endif

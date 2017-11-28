#ifndef GAMECMD_H
#define GAMECMD_H

const int ADMIN_GET_PLAY_INFO = 0x0085;         //取闲置用户

const int ADMIN_RELOAD_CONF	 = 0x0300;         //重新拉取配置

const int HALL_USER_LOGIN	            = 0x0001;                  // 用户注册信息
const int HALL_USER_LOGOUT				= 0x0002;                // 用户注消信息
const int HALL_USER_ALLOC				= 0x0003 ;                 // 用户申请桌子
const int HALL_GET_MEMBERS				= 0x0005;                  // 取各个游戏场用户数
const int HALL_MSG_CHANGETAB			= 0x0006;                  // 换桌
const int SERVER_CMD_KICK_OUT			= 0x0030;				//踢出用户


//================客户端命令=======================//
const int USER_HEART_BEAT			    = 0x0101;  //用户心跳，设置过期时间
const int CLIENT_MSG_LOGINCOMING		= 0x0200; //用户登录并且进入房间
const int CLIENT_MSG_LEAVE              = 0x0250; //用户离开
const int CLIENT_MSG_TABLEDET			= 0x0251; //用户获取牌局详细信息
const int SERVER_MSG_KICKOUT            = 0x0253; //游戏服务器踢出用户命令
const int CLIENT_MSG_LOGOUT             = 0x020C; //用户离开


//====================百人德州==========================

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

const int SERVER_GAMESERVER_CLOSED		= 0x0301; //游戏服务器异常关闭

#define EXIT			-9999
#endif


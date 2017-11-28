#ifndef _GAMECMD_H
#define _GAMECMD_H

const int PRIVATE_ROOM				= 4;
const int LEVEL1					= 1;
const int LEVEL2					= 2;
const int LEVEL3					= 3;
const int LEVEL5					= 5;

const int SERVER_MSG_ROBOT				= 0x0244; //给机器人发送详细信息
const int CLIENT_MSG_LOGINCOMING		= 0x0200; //用户登录并且进入房间
const int CLIENT_MSG_START_GAME 		= 0x0201;  //用户准备
const int CLIENT_MSG_BET_CALL			= 0x0203; //跟注
const int CLIENT_MSG_BET_RASE			= 0x0204; //加注
const int CLIENT_MSG_BET_RASE_FOR_JS	= 0x0215; //加注
const int CLIENT_MSG_ALL_IN				= 0x0205; //ALLIN
const int CLIENT_MSG_CHECK_CARD			= 0x0206; //看牌
const int CLIENT_MSG_THROW_CARD			= 0x0207; //弃牌
const int CLIENT_MSG_COMPARE_CARD		= 0x0208; //比牌
const int GMSERVER_MSG_GAMEOVER			= 0x0209; //游戏结束
const int GMSERVER_GAME_START			= 0x020A; //游戏服务器推送游戏开始
const int GMSERVER_BET_TIMEOUT			= 0x020B; //游戏服务器推送下注超时
const int GMSERVER_WARN_KICK			= 0x020D; //游戏服务器通知用户快要踢出他了
const int PUSH_MSG_FORBID				= 0x0210; //推送再玩用户禁比轮数
const int CLIENT_MSG_CHANGE_CARD		= 0x0211; //换牌
const int CLIENT_MSG_FORBIDDEN			= 0x0212; //禁比
const int CLIENT_MSG_MUL				= 0x0213; //翻倍
const int GMSERVER_MAX_NUM				= 0x0214; //更新当前闷牌的最大值
const int CLIENT_MSG_CHAT				= 0x022C; //聊天
const int CLIENT_MSG_LEAVE              = 0x0250; //用户离开
const int CLIENT_MSG_TABLEDET			= 0x0251; //用户获取牌局详细信息
const int CLIENT_MSG_LOGINCHANGE		= 0x0252; //登录并进入换桌房间
const int SERVER_MSG_KICKOUT			= 0x0253; //游戏服务器踢出用户命令
const int CLIENT_MSG_LOGOUT             = 0x020C; //用户离开
const int CLIENT_MSG_OFFLINE            = 0x0254; //用户掉线

#endif

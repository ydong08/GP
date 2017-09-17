#ifndef GAMECMD_H
#define GAMECMD_H

#include <assert.h>
#include <string.h>
#include "vector"
#include "math.h"
#include "stdlib.h"
#include "Typedef.h"
//积分类型
#define SCORE						long							//积分类型

//常量定义
#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					34									//最大索引
#define MAX_COUNT					14									//最大数目
#define MAX_REPERTORY				108									//最大库存

//扑克定义
#define HEAP_FULL_COUNT				26									//堆立全牌

#define MAX_RIGHT_COUNT				1									//最大权位DWORD个数		

#define SERVER_LEN				    32 

#define SCROE_WAN			10000
#define ROOM_LEVEL_JUNIOR   1*SCROE_WAN
#define ROOM_LEVEL_MIDDLE	50*SCROE_WAN
#define ROOM_LEVEL_SENIOR	500*SCROE_WAN

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

const int START_ROBOT	    = 0x0044;  	//调取机器人 

const int ADMIN_CMD_RELOAD                              = 0x0300;

//================客户端命令=======================//
const int USER_HEART_BEAT = 0x0101;  //用户心跳，设置过期时间

const int CLIENT_MSG_LOGINCOMING = 0x0200; //用户登录并且进入房间
const int CLIENT_MSG_START_GAME = 0x0201;  //用户准备
const int GMSERVER_MSG_SEND_CARD = 0x0202; //游戏开始推送，游戏开始，发牌
const int CLIENT_MSG_OPERATE = 0x0203;
const int CLIENT_MSG_DISPATCH_CARD = 0x0204;
const int CLIENT_MSG_OUT_CARD = 0x0205; //出牌
const int CLIENT_MSG_OPERATE_CARD = 0x0206; //牌动作，如吃，碰，杠等
const int GMSERVER_MSG_GAMEOVER = 0x0207; //游戏结束
const int CLIENT_MSG_USERAI = 0x0208; //用户托管
const int CLIENT_MSG_CHAT = 0x0209; //用户聊天
const int CLIENT_MSG_OPERATE_RESULT = 0x0210; //牌动作操作结果
const int CLIENT_MSG_LOGOUT = 0x020C; //用户离开
const int CLIENT_MSG_CHI_HU = 0x0211;
const int CLIENT_MSG_GANG_SCORE = 0x0212; //
const int GMSERVER_MSG_CHANGED_CARD = 0x0213; // 服务器推送开始换牌
const int CLIENT_MSG_CHANGED_CARD = 0x0214; // 客户端向服务器汇报，换牌准备好
const int GMSERVER_MSG_DEFINED_LACK = 0x0215; // 服务器推送开始定缺
const int CLIENT_MSG_DEFINED_LACK = 0x0216; // 客户端向服务器汇报，定缺的牌色
const int GMSERVER_MSG_PLAYING = 0x0217; // 服务器端推送消息通知所有玩家庄家正式开始出牌了
const int GMSERVER_MSG_CHANGE_RESULT = 0x0218; // 服务器端推送消息通知所有玩家换牌后的牌
const int CLIENT_MSG_LEAVE = 0x0250; //用户离开
const int CLIENT_MSG_TABLEDET = 0x0251; //用户获取牌局详细信息
const int CLIENT_MSG_LOGINCHANGE = 0x0252; //登录并进入换桌房间
const int SERVER_MSG_KICKOUT = 0x0253; //游戏服务器踢出用户命令
const int EXIT			= -9999; 

#endif

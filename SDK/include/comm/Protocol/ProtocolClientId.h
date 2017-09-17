#ifndef __PROTOCOL_CLIENT_ID_H_
#define __PROTOCOL_CLIENT_ID_H_

const int C2S_HALL_USER_LOGIN_ID        = 0x0001;   // ”√ªß◊¢≤·–≈œ¢
const int CLIENT_MSG_GET_MONEY          = 0x0260; //客户端请求获取金币

const int CLIENT_MSG_NETWORK_STATUS = 0x0601;

// int send target uid
// int  exp uid
// int  升级进度，百分数
// BYTE 是否升级
// int 当前等级（升级后的）
// BYTE 特权是否开启
// string 当前装备昵称
// string 当前装备头像
const int S2C_NOTIFY_EXP_DATA = 0x0328;           //给客户端推送经验，等级相关数据

// c2s
// int uid
// BYTE 类型 0 特权启动 1 特权装备
// if 类型 == 0
// BYTE 是否开启  0 关闭  1 开启
// else
// BYTE 是否开启  0 昵称  1 头像
// endif
// string 装备名

// s2c
// short retcode  0 success < 0 fail
// string msg
// int uid
// BYTE 类型 0 特权启动 1 特权装备
// if 类型 == 0
// BYTE 是否开启  0 关闭  1 开启
// else
// BYTE 是否开启  0 昵称  1 头像
// endif
// string 装备名
const int CLIENT_MSG_PRIVILEGE = 0x0329;     //玩家特权消息

// c2s
// int uid
// s2c
// short
// string
// int uid
// byte data count
// for count
//  byte privilege type
//  string privi name
//  int unlocklevel
//  byte use state 0 not use, 1 used
// end for
const int CLIENT_MSG_PRIVILEGE_DATA = 0x0330; //玩家特权数据

const int USER_HEART_BEAT = 0x0101;  //用户心跳，设置过期时间

#endif //End for __PROTOCOL_CLIENT_ID_H_

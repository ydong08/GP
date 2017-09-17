//
//  GameCmd.h
//  HttpSvr
//
//  Created by yuzhan on 2016/11/9.
//
//

#ifndef GameCmd_h
#define GameCmd_h

///// to moneyserver
// int uid

///// from moneyserver
// short  retcode
// string retstring
// int    uid
// int64  money
// int64  safemoney
// int    roll
// int    roll1
// int    coin
// int    exp
const int GET_MONEY = 0x0460;       // get money from moneyserver

#endif /* GameCmd_h */

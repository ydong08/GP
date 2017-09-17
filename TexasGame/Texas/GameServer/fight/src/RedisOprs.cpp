//
// Created by user on 2017/2/7.
//

#include <Game/GameApp.h>
#include "RedisOprs.h"
#include "RedisAccess.h"
#include <vector>
#include "StrFunc.h"

int RedisOprs::BackUpWinCoinRank()
{
    Rank()->DEL("coinhash");

    std::vector<std::string> result;
    bool ret = Rank()->ZREVRANGE("wincoin", 0, 99, result);
    if (!ret)
        return -1;

    for (int i = 0; i < (int)result.size(); i++)
        Rank()->HSETi("coinhash", result[i].c_str(), i+1);

    return 0;
}

int RedisOprs::ModUserWinCoin(int uid,int64_t score)//把最新金币插入redis
{
    int resScore;
    bool ret = Rank()->ZINCRBY("wincoin", uid, score, resScore);
    return 0;
}

int RedisOprs::BackUpWealth()
{
    Rank()->DEL("wealthhash");

    std::vector<std::string> result;
    bool ret = Rank()->ZREVRANGE("wealthhash", 0, 99, result);
    if (!ret)
        return -1;

    for (int i = 0; i < (int)result.size(); i++)
        Rank()->HSETi("wealthhash", result[i].c_str(), i+1);

    return 0;
}

int RedisOprs::ModUserWealth(int uid,int64_t coin)//把最新分数插入redis
{
    int resScore;
    bool ret = Rank()->ZINCRBY("wealth", uid, coin, resScore);
    return 0;
}

//获取当天完成的局数
int RedisOprs::getPlayerCompleteCount( int uid )
{
    std::string result;
    bool ret = Rank()->HGET("tx_winstreak", StrFormatA("%d", uid).c_str(), result);
    int retCount = result.size();

    return retCount;
}

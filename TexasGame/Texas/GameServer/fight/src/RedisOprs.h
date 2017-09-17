//
// Created by user on 2017/2/7.
//

#ifndef DD_GAMES_REDISOPRS_H
#define DD_GAMES_REDISOPRS_H


class RedisOprs {
public:
	static	int ModUserWinCoin(int uid,int64_t coin);	//赢取金币
	static int ModUserWealth(int uid,int64_t coin);	//财富

	static int BackUpWinCoinRank();
	static int BackUpWealth();
	//获取当天完成的局数
	static int getPlayerCompleteCount(int uid);
};


#endif //DD_GAMES_REDISOPRS_H

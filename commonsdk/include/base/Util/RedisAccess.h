#ifndef _RedisAccess_H_
#define _RedisAccess_H_

#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include "Util_struct.h"

struct DataScore {
    std::string key;
    int score;
};

class RedisTimer;
struct redisContext;
struct redisReply;

class RedisAccess {
public:
    RedisAccess();
    RedisAccess(const char *addr);
    virtual ~RedisAccess();
    /**
     * 执行redis GET 命令
     * @param szkey 键字符串
     * @param value 返回string值
     * @param szDefault 默认值
     * @return true成功，false失败
     */
    bool GET(const char *szkey, std::string &value, const char* szDefault = "");
    /**
     * 执行redis GET 命令
     * @param szkey 键字符串
     * @param value 返回int值
     * @param iDefault 默认值
     * @return true成功，false失败
     */
    bool GETi(const char *szkey, int &value, int iDefault = 0);
    /**
     * 执行redis HGET 命令
     * @param szkey 键字符串
     * @param field 子键字符串
     * @param value 返回string值
     * @param szDefault 默认值
     * @return true成功，false失败
     */
    bool HGET(const char *szkey, const char *field, std::string &value, const char* szDefault = "");
    /**
     * 执行redis HGET 命令
     * @param szkey 键字符串
     * @param field 子键字符串
     * @param value 返回int值
     * @param iDefault 默认值
     * @return true成功，false失败
     */
    bool HGETi(const char *szkey, const char *field, int &value, int iDefault = 0);
    /**
     * 执行redis HMGET 命令
     * @param szKey 键字符串
     * @param vsFields 子键字符串数组
     * @param mpRet 返回子键=>值map
     * @return true成功，false失败
     */
    bool HMGET(const char *szKey, const std::vector<std::string> &vsFields, std::map<std::string, std::string> &mpRet);
    /**
     * 执行redis HGETALL 命令
     * @param szKey 键字符串
     * @param mpRet 返回子键=>值map
     * @return true成功，false失败
     */
    bool HGETALL(const char *szKey, std::map<std::string, std::string> &mpRet);
    /**
     * 执行redis SADD 命令
     * @param szKey 键字符串
     * @param szValue 值字符串
     * @return true成功，false失败
     */
    bool SADD(const char *szKey, const char *szValue);
    /**
     * 执行redis HSET 命令
     * @param szKey 键字符串
     * @param field 子键字符串
     * @param value 值字符串
     * @return true成功，false失败
     */
    bool HSET(const char *szKey, const char *field, const char *value);
    /**
     * 执行redis HSET 命令
     * @param szKey 键字符串
     * @param field 子键字符串
     * @param value int值
     * @return true成功，false失败
     */
    bool HSETi(const char *szKey, const char *field, int value);
    /**
     * 执行redis HINCRBY 命令
     * @param szKey 键字符串
     * @param field 子键字符串
     * @param incr 增量int值
     * @param value 返回int值，表示当前的键值
     * @return true成功，false失败
     */
    bool HINCRBY(const char *szKey, const char *field, int incr, int &value);
    /**
     * 执行redis HMSET 命令
     * @param szKey 键字符串
     * @param mapKV 子键=>值map
     * @return true成功，false失败
     */
    bool HMSET(const char *szKey, std::map<std::string, std::string> &mapKV);
    /**
     * 执行redis SET 命令
     * @param szkey 键字符串
     * @param value int值
     * @param expire 过期时间(second)
     * @return true成功，false失败
     */
    bool SET(const char *szkey, int value, int expire = 0);
    /**
     * 执行redis SET 命令
     * @param szkey 键字符串
     * @param value string值
     * @param expire 过期时间(second)
     * @return true成功，false失败
     * @return
     */
    bool SET(const char *szkey, const char *value, int expire = 0);
    /**
     * 执行redis EXPIRE 命令
     * @param szkey 键字符串
     * @param expire 过期时间(second)
     * @return true成功，false失败
     */
    bool EXPIRE(const char *szkey, int expire);
    /**
     * 执行redis DEL 命令
     * @param szkey 键字符串
     * @return true成功，false失败
     */
    bool DEL(const char *szkey);
    /**
     * 执行redis INCRBY 命令
     * @param szKey 键字符串
     * @param incr 增量int值
     * @return true成功，false失败
     */
    int64_t INCRBY(const char *szKey, const int64_t incr, int expire = 0);
    /**
     * 执行redis SPOP 命令
     * @param szkey 键字符串
     * @return 返回字符串
     */
    std::string SPOP(const char *szkey);
    /**
     * 执行redis ZREVRANGE 命令
     * @param szKey 键字符串
     * @param start 开始序号
     * @param end 结束序号
     * @param vec 返回有序集合的键名数组
     * @return true成功，false失败
     */
    bool ZREVRANGE(const char *szKey, int start, int end, std::vector<std::string> &vec);
    /**
     * 执行redis ZREVRANGEWITHSCORE 命令
     * @param szKey 键字符串
     * @param start 开始序号
     * @param end 结束序号
     * @param vec 返回有序集合的键名和分值数组
     * @return true成功，false失败
     */
    bool ZREVRANGEWITHSCORE(const char *szKey, int start, int end, std::vector<DataScore> &vec);
    /**
     * 执行redis ZREVRANK 命令
     * @param szKey 键字符串
     * @param uid 用户id
     * @param rank 返回用户的等级
     * @return true成功，false失败
     */
    bool ZREVRANK(const char *szKey, int uid, int &rank);
    /**
     * 执行redis ZSCORE 命令
     * @param szKey 键字符串
     * @param uid 用户id
     * @param score 返回用户的分值
     * @return true成功，false失败
     */
    bool ZSCORE(const char *szKey, int uid, int &score);
    /**
     * 执行redis ZINCRBY 命令
     * @param szKey 键字符串
     * @param uid 用户id
     * @param increment 增涨值
     * @param score 返回用户的分值
     * @return true成功，false失败
     */
    bool ZINCRBY(const char *szKey, int uid, int increment, int &score);

    bool ping();

    bool connect();

    bool connect(const char* addr);

    void close();

    /**
     * 能处理key和value带空格的情况，请规范写法，把key和value放到最右边的...的位置
     * @param format
     * @return
     */
    redisReply *CommandV(const char *format, ...);

    redisReply *CommandArgv(std::vector<const char *> &vtAll);

    int appendCommand(const char *format, ...);

    redisReply *applyBatchCommand();

private:
    redisContext *m_pRedis;
    AddressInfo m_addrRedis;
    RedisTimer  *m_pTimerPing;
};


#endif



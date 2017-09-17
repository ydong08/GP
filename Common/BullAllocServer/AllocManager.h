#ifndef _AllocManager_H_
#define _AllocManager_H_
#include <map>

#define STATUS_TABLE_CLOSE	-1 //桌子关闭
#define STATUS_TABLE_EMPTY	 0 //桌子空
#define STATUS_TABLE_READY	 1 //桌子正在准备

#define STATUS_TABLE_ACTIVE 2	//桌子正在玩牌
#define STATUS_TABLE_OVER	 3	//桌子游戏结束

const int MAX_TABLE_USER = 5;

typedef struct GameTable
{
	int		m_nTid;
	short	m_nServerId;
	short	m_nLevel;
	short	m_nStatus;//桌子的状态 
	short	m_nUserCount;
	char	m_szName[64];
	int		m_nOwner;
	bool	m_bhaspasswd;
	short	m_MaxCount;
	int64_t m_Money;
	GameTable() {}
	GameTable(int tid, short svid, short level):m_nTid(tid), m_nServerId(svid), m_nLevel(level)
	{
		m_nStatus = 0;
		m_bhaspasswd = false;
		m_MaxCount = MAX_TABLE_USER;
		m_Money = 0;
	}
}GameTable;

typedef std::map<int , GameTable*> TableListMap;

class AllocManager
{
	public:
		static AllocManager* getInstance();
		virtual ~AllocManager() {};
		int addTable(int tid, short svid, short level, short usercount, int64_t money = 0);
		GameTable* getTable(int tid);
		void delTablesInSvid(short svid);
		GameTable* getAllocTable(short level = 0, int tid = 0, int64_t money = 0);
		GameTable*  getListTable(int count, int tid = 0);
		GameTable* getHighTable(short level = 0, int tid = 0, int64_t money = 0);
		void updateTableUserCount(short svid, int tid, short level, short tableUserCount, int64_t money = 0);
		void setTableStatus(short svid, int tid, short level, short status);
		GameTable* createPrivateTable(short level);
		void getPrivateList(TableListMap &list);
	private:
		bool isaccordwithMoney(int64_t currmoney, int64_t money);
	protected:
		TableListMap _tablelistMap;//存储整个alloc里面放的桌子信息
		TableListMap _tableUserCountMap[MAX_TABLE_USER + 1];//存储匹配信息 分人数放到不同index下的标下


	private:
		AllocManager() {};
};
#endif

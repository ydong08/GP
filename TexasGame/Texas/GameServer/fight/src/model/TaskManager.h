#ifndef TaskManager_H
#define TaskManager_H
#include "GameLogic.h"

#define MAX_TASK 128
#define EASY_MAX 32

typedef struct _Task
{
	int64_t taskid;
	char taskname[64];
	short ningotlow;
	short ningothigh;
}Task;


class Player;
class Table;
class TaskManager
{
	public:
		virtual ~TaskManager(){};
		
		static TaskManager * getInstance();
		int init(short loadtype);
		int loadTask(short loadtype);
		Task * getTask();
		Task * getRandTask();
		Task * getRandEsayTask();
		bool calcWinOrAllin(Player* player, Table* table, short winflag);
		bool calcCardType(Player* player, Table* table, short cardtype);
		bool calcCardValue(Player* player, Table* table, short cardnum, short cardvalue);
		bool calcBottomType(Player* player, Table* table, short bottomtype);
		bool calcBottomValue(Player* player, Table* table, short bbovalue);
		bool calcColor(Player* player, Table* table, short bcolor);
		bool calcPlayerComplete(Player* player, Table* table);
		int getNewTask(Player* player);
		bool isEsayTask(int64_t taskid);
	private:
		TaskManager();
		Task taskarry[MAX_TASK];
		Task* taskEsay[EASY_MAX];
		//任务总数
		short countTask;
		//当前派发任务的下标
		short index;
		//简单任务的个数
		short countEasy;
};



#endif


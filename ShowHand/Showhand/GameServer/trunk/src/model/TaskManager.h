#ifndef TaskManager_H
#define TaskManager_H
#include "GameLogic.h"

#define MAX_TASK 128
#define EASY_MAX 32

typedef struct _Task
{
	int64_t taskid;
	char taskIOSname[64];
	char taskANDname[64];
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
		int init();
		int loadTask();
		Task * getTask();
		Task * getRandTask();
		Task * getRandEsayTask();
		bool calcPlayerComplete(Task* task, Player* player, Table* table);
		int setPlayerComplete(Player* player);
		int setNewTask(Player* player, int &boardtask);
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


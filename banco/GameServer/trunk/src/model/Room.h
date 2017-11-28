#ifndef _ROOM_H
#define _ROOM_H

#include "Player.h" 
#include "Table.h"
#include "CDL_Timer_Handler.h"

#define MAX_ROOM_TABLE 500

class Room;
class RoomTimer:public CCTimer
{
	public:
		RoomTimer(){} ;
		virtual ~RoomTimer() {};
		void init(Room* room);
		void startHeartTimer();
		void stopHeartTimer();
private:
		Room* room;
		int ProcessOnTimerOut();
		int HeartTimeOut();	
};

class Room
{
	public:
		virtual ~Room(){};
		
		static Room * getInstance();
		int init();
		Table* getAvailableTable();
		Table* getAvailableTable(int table_id);
		Table* getTable(int table_id);
		Player* getAvailablePlayer();
		Table* getChangeTable(int changeTid);
		Player* getPlayerUID(int uid);

		inline int getMaxTableCount(){return max_table_count;}; 
		inline Table* getAllTables(){return tables;}; 
		inline Player* getAllPlayers(){return players;}; 

		inline int getMaxUsers() {return max_user_count;};
		inline int getMaxTables(){return max_table_count;};
		inline int getCurrUsers(){return curr_user_count;};
		inline void setCurrUsers(int n){this->curr_user_count=n;if(n>max_count) max_count=n ;};
		inline void setCurrTables(int n){this->curr_table_active=n;if(n>max_count_tab) max_count_tab=n ;};
		inline int getCurrTables(){return curr_table_active;};

		inline int getIPhoneUsers(){return iphone_count;};
		inline int getAndroidUsers(){return android_count;};
		inline int getIPadUsers() {return ipad_count;};
		inline int getRobotUsers(){return robot_count;};
		inline int getMaxCount(){return max_count;};
		inline int getMaxCountTab(){return max_count_tab;};

		inline short getStatus(){return m_nStatus;};
		inline short setStatus(short m_nStatus){return this->m_nStatus = m_nStatus;};
        
        
		void startHeartTimer();
		void stopHeartTimer();
		
	public:
		Table tables[MAX_ROOM_TABLE];
		Player players[MAX_ROOM_TABLE*GAME_PLAYER];
		int Ready_time_out;
        int monitor_time;
        int keeplive_time;

	private:
		Room();
		int max_user_count;
		short max_table_count;
		int curr_user_count;
		short curr_table_active;
		short m_nStatus;
		
		short iphone_count;
		short android_count;
		short ipad_count;
		short robot_count;
		short max_count;
		short max_count_tab;
		RoomTimer timer;		

	friend class RoomTimer;
};

#endif


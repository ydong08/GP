#ifndef _HallManager_H_
#define _HallManager_H_

#include <map>

class ClientHandler; 
class OutputPacket;
class HallManager
{
	public:
		static HallManager* getInstance();
		virtual ~HallManager() {};
		int sendToHall(int hall,  OutputPacket* outPacket, bool isEncrypt=true);
		int addHallHandler(int hallid,ClientHandler* hall);
		ClientHandler*  getHallHandler(int hallid);
		void  delHallHandler(int hallid);	
	private:
		std::map<int , ClientHandler*> hallMap;
		HallManager() {};
};
#endif


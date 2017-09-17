#ifndef _HallManager_H_
#define _HallManager_H_
#include "ClientHandler.h"
#include <map>


class HallManager
{
	public:
		static HallManager* getInstance();
		virtual ~HallManager() {};
		int sendToHall(int hall,  OutputPacket* outPacket, bool isEncrypt=true);
		int sendAllHall(OutputPacket* outPacket, bool isEncrypt=true);
		int addHallHandler(int hallid,ClientHandler* hall);
		ClientHandler*  getHallHandler(int hallid);
		void  delHallHandler(int hallid);

	protected:	
		std::map<int , ClientHandler*> hallMap;

	private:
		HallManager() {};
};
#endif

#ifndef _HallManager_H_
#define _HallManager_H_
#include <map>

class BaseClientHandler;
class OutputPacket;

class HallManager
{
	public:
		static HallManager* getInstance();
		virtual ~HallManager() {};
		int sendToHall(int hall,  OutputPacket* outPacket, bool isEncrypt=true);
		int sendAllHall(OutputPacket* outPacket, bool isEncrypt=true);
		int addHallHandler(int hallid, BaseClientHandler* hall);
		BaseClientHandler*  getHallHandler(int hallid);
		void  delHallHandler(int hallid);

	protected:	
		std::map<int , BaseClientHandler*> hallMap;

	private:
		HallManager() {};
};
#endif

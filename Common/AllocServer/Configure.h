
#ifndef OPTIONS_H
#define OPTIONS_H

#include <time.h>
#include <string>
#include "Typedef.h"
#include "Game/GameConfig.h"

class Configure : public GameConfig
{
public:
	static Configure* getInstance() {
		static Configure * options = NULL;
		if (options == NULL)
			options = new Configure();
		return options;
	}

public:
	bool LoadGameConfig();

	~Configure();

public:
	//uint16_t port;
	std::string name;
	uint16_t numplayer;

	int highlimit;
	uint16_t mul;
	uint16_t matchtime;
	uint16_t allowrobot;
	uint16_t norobot;

	//short level;
private:
	Configure();
};

#endif

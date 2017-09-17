
#ifndef OPTIONS_H
#define OPTIONS_H

#include <time.h>
#include <unistd.h>
#include "Typedef.h"
#include <string>
#include "Game/GameConfig.h"

class Configure : public GameConfig
{
public:

	static Configure* instance() {
		static Configure * options = NULL;
		if(options==NULL)
			options = new Configure();
		return options;
	}

	virtual bool LoadGameConfig();

//***********************************************************
public:
	const char* ServerName();

	short numplayer;

	time_t lasttime;

	int highlimit;
	short mul;
private:
	Configure();
};

#define _CFG_(key)  Configure::instance()->key  
#endif

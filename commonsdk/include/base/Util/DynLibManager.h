

#pragma once


#include <map>
#include <string>

class DynLib;

class DynLibManager
{
public:
	DynLibManager();
	~DynLibManager();

public:
	DynLib* Load(const std::string& filename);
	void Unload(DynLib* lib);

private:
	typedef std::map<std::string, DynLib*> DynLibList;
	DynLibList mDynLibList;

private:
	static DynLibManager* instance;

public:
	static DynLibManager* GetSingleton();
};


#pragma once


#include <string>

class DynLib
{
protected:
	std::string mName;
	/// Gets the last loading error
	std::string dynlibError(void);
public:
	DynLib(const std::string& name);
	~DynLib();

	void Load();
	void Unload();
	const std::string& GetName(void) const { return mName; }

	void* GetSymbol(const std::string& strName) const throw();

protected:
	void* mInst;
};
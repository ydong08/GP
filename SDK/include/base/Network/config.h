#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>
#include <string.h>
#include <string>
#include <map>

#include <noncopyable.h>

class CConfig : private noncopyable
{
public:
	CConfig() {};
	~CConfig() {};

    static CConfig* Instance (void);
    static void Destroy (void);

	int GetIntVal(const char *sec, const char *key, int def=0);
	unsigned long long GetSizeVal(const char *sec, const char *key, unsigned long long def=0, char unit=0);
	int GetIdxVal(const char *, const char *, const char * const *, int=0);
	const char *GetStrVal (const char *sec, const char *key);
	bool HasSection(const char *sec);
	bool HasKey(const char *sec, const char *key);
	void Dump (FILE *fp, bool dec=false);
	int Dump (const char *fn, bool dec=false);
	int ParseConfig(const char *f=0, const char *s=0);
	
private:
	struct nocase
	{
		bool operator()(const std::string &a, const std::string &b) const
		{ return strcasecmp(a.c_str(), b.c_str()) < 0; }
	};

	typedef std::map<std::string, std::string, nocase> keymap_t;
	typedef std::map<std::string, keymap_t, nocase> secmap_t;

private:
	std::string filename;
	secmap_t smap;
};

#endif

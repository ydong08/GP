#ifndef ___STR_FUNC___HHH___
#define ___STR_FUNC___HHH___

#include <string>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <regex.h>

#define SPACES 						" \t\r\n"
#define HYPHENP(x)  				(*(x) == '-' && !*((x) + 1))
#define MINVAL(x, y) 				((x) < (y) ? (x) : (y))
#define safe_delete(p) 				if ((p)) { delete((p));(p) = 0; }
#define safe_free(p) 				if ((p)) { free((p));(p) = 0; }

#define SIZEOFARRAY(array)			(sizeof(array)/sizeof(*(array)))

#define ISSPACE(x)  				isspace ((unsigned char)(x)) //
#define ISDIGIT(x)  				isdigit ((unsigned char)(x))//当c为数字0-9时，返回非零值
#define ISWDIGIT(x)  				iswdigit ((unsigned short)(x))//当c为数字0-9时，返回非零值
#define ISXDIGIT(x)  				isxdigit ((unsigned char)(x))//当c为A-F,a-f或0-9之间的十六进制数字时，返回非零值，否则返回零。
#define ISALPHA(x)  				isalpha ((unsigned char)(x))//az或AZ
#define ISALNUM(x)  				isalnum ((unsigned char)(x))//否为英文字母或阿拉伯数字 = isalpha(c) || isdigit(c)
#define TOLOWER(x)  				((char)tolower ((unsigned char)(x)))
#define TOUPPER(x) 					((char)toupper ((unsigned char)(x)))
#define IGNORE_SPACE(p) 			for (;*(p)!='\0' && ISSPACE(*(p));(p)++);
#define FIND_SPACE(p) 				for (;*(p)!='\0' && !ISSPACE(*(p));(p)++);

#define STRDUP(src)					strdup((src) ? (src) : "" )
#define ISASCII(x)  				isascii ((unsigned char)(x))
#define STRLEN(src)					((src) ? strlen(src) : 0 )

#ifndef WIN32
#define _atoi64(val) strtoll(val, NULL, 10)
#endif
#define atoi64(val) _atoi64(val)


unsigned int GetRandInt(unsigned int nMin, unsigned int nMax);
char GetRandHexChar(bool bLower=true);
char GetRandChar(bool bLower=true);
char IntToHexChar(unsigned int ucValue, bool bLower=true);
char IntToChar(unsigned int ucValue, bool bLower=true);

//从1234567890abcdefghijklmnopqrstuvwxyz获取指定长度的随机字符串
std::string GetRandString(int len);

std::string GetRandStringOnly(int len);
//截取strSrc 从pos 到 strFind的内容，如果strFind没有找到，则取pos到结束的内容
std::string GetMidStr(const std::string &strSrc, std::string::size_type &pos, const std::string strFind);
//截取strSrc，从pos位置开始查找strBegin，到strEnd结束，其中strBegin可以带通配符，strEnd不能带通配符
std::string GetSubStr(const std::string strSrc, std::string::size_type &pos,const std::string strBegin, const std::string strEnd);
//获取左/右字符串
std::string GetLRStr(const std::string strSrc, const std::string strFind, bool bRight=false, bool bReverse=false);
std::string TrimStr(std::string strSrc);

//获取带通配符串的真实内容
std::string GetReallyContent(const std::string &strSource, const std::string &strFind, const std::string::size_type bgPos = 0);
//strReplaceSrc支持通配符
bool StrReplaceAll( std::string &strSrc, const std::string strReplaceSrc, const std::string strReplaceDst );
//strReplaceSrc支持通配符
bool StrReplaceOnce( std::string &strSrc, const std::string strReplaceSrc, const std::string strReplaceDst, const std::string::size_type bgPos = 0 );

std::string StrFormatA(const char *fmt, ...);
//std::string VStrFormatA(const char *fmt, va_list pargs);
std::wstring StrFormatW(const wchar_t *fmt, ...);


char * GetCharset(const char *str);		
char * FormatCharset( const char * str );

//char* GetCurDir(char * szPath,int len);

void ToLower( std::string& str );
void ToUpper( std::string& str );

bool regex_match(const std::string& strMatch, const std::string& strPattern, int nType = REG_EXTENDED | REG_NOSUB);

#endif

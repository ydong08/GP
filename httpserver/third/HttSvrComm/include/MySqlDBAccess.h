#ifndef _MySqlDBAccess_H_
#define _MySqlDBAccess_H_
#include "mysql.h"
#include "errmsg.h"
#include <iostream>
#include <map>
#include <stdint.h>

class MySqlResultSet;

class MySqlDBAccess
{
	public:
		MySqlDBAccess();
		virtual ~MySqlDBAccess() ;

		int Connect(const char*  host , const char*  user, const char*  passwd);
		int Connect(const char*  host , short port, const char*  user, const char*  passwd);
		int Connect(const char*  host , const char*  user, const char*  passwd, const char* db);
		int Connect(const char*  host , short port, const char*  user, const char*  passwd, const char* db);
 
		int UseDb(const char* db);
		MySqlResultSet*  Query(const char* sql);
		unsigned long   ExeSQL(const char* sql);
		void   ExeSQLNoRet(const char* sql);
		MySqlResultSet*   ExeMultiSQL(const char* sql);
        //返回insert id;
        inline unsigned long GetLastInsertId() { return mysql_insert_id(&mysql); }
		inline unsigned int   getLastErrorCode(){return mysql_errno(&mysql);};
		inline const char * getLastErrorMsg(){return mysql_error(&mysql);};
		char* ConvertBinaryToString(char* pBinaryOut, char* pBinaryData, int nLen);

		std::string EscapeString( const char* pData, int nLen );
        inline std::string EscapeString( const std::string data ) {return EscapeString(data.c_str(), data.length());}

	private:
    	int Init(const char*  host , const  char*  user, const char*  passwd );
		char  host[128];
		char  user[128] ;
		char  passwd[128] ;
		MYSQL mysql;  
};

 class MySqlResultSet
 {
	public:
		MySqlResultSet(MYSQL_RES *res);
		~MySqlResultSet();
		bool hasNext();
        int getFieldColumn(const char* fieldname);
        char* getCharValue(int column);
        char* getCharValue(const char* fieldname) { return getCharValue(getFieldColumn(fieldname)); }
        int64_t getInt64Value(int column);
        int64_t getInt64Value(const char* fieldname) { return getInt64Value(getFieldColumn(fieldname)); }
        int getIntValue(int column);
        int getIntValue(const char* fieldname) { return getIntValue(getFieldColumn(fieldname)); }
        double getFloatValue(int column);
        double getFloatValue(const char* fieldname) { return getFloatValue(getFieldColumn(fieldname)); }
        int getBinaryValue(int column,char* pBinaryBuff);
        int getBinaryValue(const char* fieldname,char* pBinaryBuff) { return  getBinaryValue(fieldname, pBinaryBuff); }

		unsigned long getRowNum();
		unsigned int getColumnNum (){return mysql_num_fields(res);};
		inline MYSQL_RES *getMYSQL_RES(){return res;};
	private:
		MYSQL_RES *res;
		MYSQL_ROW row; 
		//int init(MYSQL* mysql, MYSQL_RES *res);
		//friend class MySqlDBAccess;//友类的声明

 };
#endif


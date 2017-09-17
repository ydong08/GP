//---------------------------------------------------------------------------

#ifndef MyLoggerH
#define MyLoggerH
#include <stdio.h>
//---------------------------------------------------------------------------
class  MyLogger
{
   public :
        MyLogger();
        MyLogger(char*  filePath);
        void setLogFile(const char* logfile);
        void setLevel(int level);
        void setLevel(const char* level);
        
        void Log(const int Level, const char* Message);
        void Log(const char* Level, const char* Message) ;

		void Error(const char* format,  ...) ;
        void Waring(const char* format,  ...) ;
        void Info(const char* format,  ...) ;
        void Debug(const char* format,  ...) ;
		void Track(const char* format,  ...) ;

        static const int OFF =  6;
        static const int ERRO =  5;
        static const int WARING = 4;
        static const int INFO =  3;
        static const int DE_BUG =  2;
		static const int TRACK =  1;
        static const int ALL=   0;

   private :
        char LogFile[256];
        char Time[32];
        
        int Level;
        char* GetTimeStr();
        bool WriteToFile(const char* sFile, const char* sLevel, const char* sTime, const char* sBuf, const char* sMode="a");
        int StringToLevel(const char* sLevel);
        const char* LevelToString(const int vLevel);
        void  Init();
/*
        CRITICAL_SECTION _csLock;
        void  Lock()    { ::EnterCriticalSection( & _csLock); }
        void  Unlock()   { ::LeaveCriticalSection( & _csLock); }
*/
};
#endif



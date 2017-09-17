
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "MyLogger.h"


#define MAX_MSFBUFF_SIZE 1024*1024
static char msgbuff[MAX_MSFBUFF_SIZE]={'\0'};
int CONSOLE_OUT = 0;

 MyLogger::MyLogger()
 {
    Init();
 }

 MyLogger::MyLogger(char*  filePath)
 {
     Init();
     strcpy(LogFile , filePath);
 }
 void MyLogger::Init()
 {
   /* char  sBufferLevel[10]={'0'};
    char  sBufferLogFile[128]={'0'};
    char  sBufferConsoleOut[10]={'0'};
    read_profile_string( "log", "level",sBufferLevel, 10,"OFF", "./log.ini");
    read_profile_string( "log", "out",sBufferLogFile, 128,"log.log", "./log.ini");
    read_profile_string( "log", "console",sBufferConsoleOut, 10,"false", "./log.ini");    
    if(strcmp(sBufferConsoleOut,"true")==0)
	CONSOLE_OUT = 1;
    else
	CONSOLE_OUT = 0;
    string sTLevel = string(sBufferLevel);
    Level = StringToLevel(sTLevel) ;
    if(-1 == Level)
       Level = OFF;
    LogFile = string(sBufferLogFile);
    //if(LogFile.find_first_of(":")==-1)
    //   LogFile = GetExeDir()+ LogFile;
    */
 }

 void MyLogger::setLogFile(const char* logfile)
 {
    strcpy(LogFile , logfile);
 }
void MyLogger::setLevel(int level)
{
  this->Level = level;
}
void MyLogger::setLevel(const char* level)
{
    this->Level = StringToLevel(level) ;
	 
    if(-1 == this->Level)
       this->Level = OFF;
	 
}

 void MyLogger::Log(const int Level, const char* Message)
 {
    if(Level>OFF || Level<ALL)
      return;
    if(Level >= this->Level)
    {
       const char* sLevel = LevelToString(Level);
       char* sTime = GetTimeStr();
      // sprintf(msgbuff,"[%s] %s %s\n",sLevel,sTime,Message);
       //string TempMessage = "[" + sLevel + "]" + "" +sTime+ " "+ Message+"\n";
       // Lock();
		//if(CONSOLE_OUT)
		//	printf("[%s] %s %s",sLevel,sTime,msgbuff);
            
       this->WriteToFile(LogFile,sLevel,sTime,msgbuff);
       // Unlock();

    }
 }

  void MyLogger::Log(const char* Level, const char* Message)
  {
      int iLevel = StringToLevel(Level);
      Log(iLevel,Message);
  }

 bool MyLogger::WriteToFile(const char* sFile, const char* sLevel, const char* sTime , const char* sBuf, const char* sMode )
 {
    bool ret = false;
    size_t msglen = strlen(sBuf);
    FILE* fp = fopen(sFile, sMode);
    if (fp != NULL)
    {
        char head[32];
        sprintf(head,"[%s] [%s] ",sLevel,sTime);
        fwrite(head, sizeof(char), strlen(head), fp);
		printf("%s%s",head,sBuf);		
        size_t len = fwrite(sBuf, sizeof(char), msglen, fp);
        if (len ==  msglen)
            ret = true;
        fclose(fp);
    }

    return ret;
 }
 int MyLogger::StringToLevel(const  char* sLevel)
 {
    if(sLevel==NULL)
        return -1;
    char strLever[16];
    char* p=strLever;
    while ((*(p++) = toupper(*sLevel++)) != '\0')
        ;
    if(strcmp(strLever,"OFF")==0) return  OFF;
    else if(strcmp(strLever,"ERRO")==0) return  ERRO;
    else if(strcmp(strLever,"ERROR")==0) return  ERRO;
    else if(strcmp(strLever,"WARING")==0) return  WARING;
    else if(strcmp(strLever,"INFO")==0) return  INFO;
    else if(strcmp(strLever,"DEBUG")==0) return  DE_BUG;
	else if(strcmp(strLever,"TRACK")==0) return  TRACK;
    else if(strcmp(strLever,"ALL")==0) return  ALL;
    else    return  -1;
 }
 const char* MyLogger::LevelToString(const int  Level)
 {
    int i = Level;
    switch(i)
    {
      case OFF:return "OFF";
      case ERRO:return "ERROR";
      case WARING:return "WARIN";
      case INFO:return "INFOR";
      case DE_BUG:return "DEBUG";
	  case TRACK:return "TRACK";
      case ALL:return " ALL ";
      default: return "NULL_LEVEL";
    }
 }

 // yyyy-mm-dd hh:nn:ss
 char* MyLogger::GetTimeStr()
 {
    time_t t;
    time(&t);
    strftime(Time,  20, "%Y-%m-%d %H:%M:%S",localtime(&t));
    return Time ;
 }

void MyLogger::Error(const char* format,  ...) 
{
		va_list ap; 
		va_start(ap, format);
		int n = vsnprintf(msgbuff,MAX_MSFBUFF_SIZE,format, ap); 
		va_end(ap);
		//if(msgbuff[n-1]=='\n')
		//	msgbuff[n-1]='\0';
		Log(ERRO,msgbuff);
}
void MyLogger::Waring(const char* format,  ...) 
{
		va_list ap; 
		va_start(ap, format); 
		int n = vsnprintf(msgbuff,MAX_MSFBUFF_SIZE,format, ap); 
		va_end(ap);
		//if(msgbuff[n-1]=='\n')
		//	msgbuff[n-1]='\0';
		Log(WARING,msgbuff);
}
void MyLogger::Info(const char* format,  ...)
{
		va_list ap; 
		va_start(ap, format); 
		int n = vsnprintf(msgbuff,MAX_MSFBUFF_SIZE,format, ap); 
		va_end(ap); 
		//if(msgbuff[n-1]=='\n')
		//	msgbuff[n-1]='\0';
		Log(INFO,msgbuff);
}
void MyLogger::Debug(const char* format,  ...) 
{
		va_list ap; 
		va_start(ap, format); 
		int n = vsnprintf(msgbuff,MAX_MSFBUFF_SIZE,format, ap); 
		va_end(ap); 
	   //	if(msgbuff[n-1]=='\n')
	   //		msgbuff[n-1]='\0';
		Log(DE_BUG,msgbuff);
}
void MyLogger::Track(const char* format,  ...) 
{
		va_list ap; 
		va_start(ap, format); 
		int n = vsnprintf(msgbuff,MAX_MSFBUFF_SIZE,format, ap); 
		va_end(ap); 
	   //	if(msgbuff[n-1]=='\n')
	   //		msgbuff[n-1]='\0';
		Log(TRACK,msgbuff);
}

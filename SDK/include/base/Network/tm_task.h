#ifndef TIME_MGR_TASK
#define TIME_MGR_TASK
 #include <sys/time.h>
void get_timetask(const char* tmfile="./timemgrtask");
void get_timeofday(struct timeval* pval,const char* tmfile="./timemgrtask");
int get_time(const char* tmfile="./timemgrtask");
int getdelay(const struct timeval& begin);

#endif

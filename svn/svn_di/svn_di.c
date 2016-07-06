#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
	char str_ci[2048] = {0};
	char str_name[256] = {0};
	char str_commit[1024] = {};
	time_t time_ci;
	struct tm *p_time;
	
	time(&time_ci);
	p_time = localtime(&time_ci);

	//printf("请输入姓名：");
	//scanf("%s",str_name);
	printf("请输入修改动作：");
	scanf("%s",str_commit);

	sprintf(str_ci,"svn ci -m \"上传人:潘长涛;动作:%s;时间:%d/%02d/%02d-%02d:%02d:%02d\"",
		str_commit,(1900+p_time->tm_year),p_time->tm_mon,p_time->tm_mday,p_time->tm_hour,p_time->tm_min,p_time->tm_sec);	

	printf("%s\n",str_ci);
	system (str_ci);

	return 0;
}

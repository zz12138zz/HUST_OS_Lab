#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

 
void print(  struct dirent *entry){
    struct stat buf;
    lstat(entry->d_name,&buf);
    char filemode[11];
    filemode[10]='\0';
    int i=0;
    while (i < 9) {
        if (buf.st_mode & (1 << i)) {
            switch (i % 3) {
                case 0:
                    filemode[9 - i] = 'x';
                    break;
                case 1:
                    filemode[9 - i] = 'w';
                    break;
                case 2:
                    filemode[9 - i] = 'r';
                    break;
            }
        } else {
            filemode[9 - i] = '-';
        }
        i++;
    }
    if(S_ISDIR(buf.st_mode))//目录文件
        filemode[0]='d';
    else if(S_ISREG(buf.st_mode))//普通文件
        filemode[0]='-';
    else if(S_ISCHR(buf.st_mode))//字符文件
        filemode[0]='C';
    else if(S_ISBLK(buf.st_mode))//块文件
        filemode[0]='b';
    else if(S_ISFIFO(buf.st_mode))//管道文件
        filemode[0]='p';
    else if(S_ISLNK(buf.st_mode))//链接文件
        filemode[0]='l';
    else if(S_ISSOCK(buf.st_mode))//套接字文件
        filemode[0]='s';
    printf("%-s ",filemode);
    printf("%-ld ",buf.st_nlink);
    printf("%-s  ",getpwuid(buf.st_uid)->pw_name);
    printf("%-s  ",getgrgid(buf.st_gid)->gr_name);
    printf("%-7ld ",buf.st_size);
    struct tm t;
    char date_time[64];
    time_t time = buf.st_mtime;
    strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", localtime_r(&time, &t));
    printf("%-s ",date_time);
	printf(" %-15s\n",entry->d_name);
}

void printdir(char *dir, int depth){
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if  ((dp = opendir(dir)) == NULL){
         printf("open dir erorr\n");
         return;
    }
    chdir(dir);
    while((entry=readdir(dp))!=NULL ){
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)){
            if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0)
                continue;
            for(int i=0;i<depth;i++){
                printf(" ");
            }
            print(entry);
            printdir(entry->d_name,depth+4);
        }
        else{
            for(int i=0;i<depth;i++){
                printf(" ");
            }
            print(entry);
        }
    }
    chdir("..");
    closedir(dp);
    return;
}

int main(int argc,char* argv[])
{
	if(argc == 1) 
	{
		printf("input path after main!\n");
	}
	else
	printdir(argv[1],0);
	return 0;
}

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>//read,write
#include<fcntl.h>//open
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#define BUF_SIZE 1024

//copy <src> <target>
int main(int argc,char **argv)
{
    int source,target;
    int rd_len,wr_len;
    char buf[BUF_SIZE];//缓冲区
    char *ptr;
    if(argc!=3){
        fprintf(stderr,"Input:copy <src> <target>\n");
        exit(1);
    }
    //read
    if((source=open(argv[1],O_RDONLY))==-1){
        //只读模式
        fprintf(stderr,"Open %s error:%s\n",argv[1],strerror(errno));
        exit(1);
    }
    //write
    if((target=open(argv[2],O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))==-1){
        //创建|写 模式，可读可写权限
        fprintf(stderr,"Open %s error:%s\n",argv[2],strerror(errno));
        exit(1);
    }
    //copy
    while(rd_len=read(source,buf,BUF_SIZE)){
        if(rd_len==-1&&errno!=EINTR)break;//发生错误，文件读写位置无法预期，且不是因为慢系统调用
        else if(rd_len>0){
            ptr=buf;
            while(wr_len=write(target,ptr,BUF_SIZE)){
                if(wr_len==-1&&errno!=EINTR)break;
                else if(wr_len==rd_len)break;
                else if(wr_len>0){//只写了一部分
                    ptr+=wr_len;
                    rd_len-=wr_len;
                }
            }
            if(wr_len==-1)break;
        }
    }
    close(source);
    close(target);
    return 0;
}
#include<linux/unistd.h>
#include<stdio.h>
#include<asm/unistd.h>

int main(int argc,char **argv){
    int i=syscall(385,argv[1],argv[2]);
    printf("Done:i=%d\n",i);
    return 0;
}
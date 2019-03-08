#include <sys/types.h> 
#include <sys/stat.h> 
#include <string.h> 
#include <stdio.h> 
#include <fcntl.h> 
 
int main(void) 
{ 
    int fd; 
    char buf[1024]; 
    char get[1024]; 
 
    memset(get, 0, sizeof(get)); 
    memset(buf, 0, sizeof(buf)); 
    printf("please enter a string you want input to mydev:\n"); 
    gets(get); 
 
    fd = open("/dev/mydev", O_RDWR, S_IRUSR|S_IWUSR);//打开设备 
 
    if (fd > 0) { 
        read(fd, &buf, sizeof(buf)); 
        printf("The message in mydev now is: %s\n", buf); 
 
        //将输入写入设备 
        write(fd, &get, sizeof(get)); 
        //读出设备的信息并打印 
        read(fd, &buf, sizeof(buf)); 
        printf("The message changed to: %s\n", buf); 
        sleep(1); 
    }  
    else { 
        printf("Error...try add sudo...\n"); 
        return -1; 
    } 
 
    close(fd);//释放设备 
 
    return 0;

}
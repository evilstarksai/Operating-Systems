#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    int fd[2];
    pipe(fd);
    //int fd=open("temp",O_RDWR);
    char s;
    int pid=fork();
    if(!pid){
        while(read(fd[0],&s,1)==0){
            printf("1\n");
            continue;
        }
        printf("first i(child) got killed\n");
    }
    else{
        sleep(10);
        printf("%d and no its me\n",write(fd[1],"t",1));
    }
}
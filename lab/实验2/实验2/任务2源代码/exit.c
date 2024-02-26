#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
int main()
{
    pid_t pid;
    pid = fork();
    if (pid > 0)
    {
        sleep(3);
        printf("father_pid:%d exit\n", getpid());
        exit(0);
    }
    if (pid == 0)
    {
        int i = 0;
        while ((i++) < 10)
        {
            printf("son_pid:%d\n", getpid());
            sleep(1);
        }
    }
}

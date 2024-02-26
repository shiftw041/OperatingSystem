#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <wait.h>
int main()
{
    pid_t pid;
    int status;
    int result;
    pid = fork();
    if (pid < 0)
    {
        printf("create son failed!\n");
        return 0;
    }
    if (pid > 0)
    {
        printf("father_pid:%d\n", getpid());
        result = wait(&status);
        if (result)
        {
            printf("son_pid:%d exit\n", result);
            printf("son exit code:%d\n", WEXITSTATUS(status));
        }
        else
        {
            printf("son exit failed\n");
        }
    }
    if (pid == 0)
    {
        printf("create son_pid:%d\n", getpid());
        sleep(5);
        exit(131);
    }
}

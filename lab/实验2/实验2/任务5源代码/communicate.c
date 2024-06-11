#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
void handler(int arg)
{
    printf("child:%d Bye, World!\n", getpid());
    exit(0);
}

int main()
{
    pid_t pid;
    int shm_id;
    int *share_mem;
    // 创建共享内存
    shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    pid = fork();
    if (pid == 0)
    {
        signal(SIGUSR1, handler); // 自定义信号操作
        share_mem = (int *)shmat(shm_id, 0, 0);
        *share_mem = getpid();
        shmdt(share_mem);
        while (1)
        {
            printf("Child:%d I am Child Process, alive!\n", getpid());
            sleep(2);
        }
    }
    if (pid > 0)
    {
        char input;
        int child_pid;
        sleep(2);
        share_mem = (int *)shmat(shm_id, 0, 0);
        child_pid = *share_mem;
        shmdt(share_mem);
        shmctl(shm_id, IPC_RMID, NULL);
        while (1)
        {
            kill(child_pid, SIGSTOP); // 停止子进程
            printf("Father:%d Do you want to kill Child Process:%d ?[Y/N]", getpid(), child_pid);
            input = getchar();
            getchar();
            if (input == 'y' || input == 'Y')
            {
                kill(child_pid, SIGCONT);
                kill(child_pid, SIGUSR1); // 终止子进程
                wait(NULL);
                break;
            }
            kill(child_pid, SIGCONT); // 让停止的子用户继续进行
            sleep(2);
        }
    }
    return 0;
}
// https://www.cnblogs.com/frisk/p/11602973.html signal参数
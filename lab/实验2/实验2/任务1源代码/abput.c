#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
void *pthread_A(void *arg)
{
    for (int i = 0; i <= 1000; ++i)
    {
        printf("A:%04d\n", i);
        sleep(0.5);
    }
    pthread_exit(NULL);
}
void *pthread_B(void *arg)
{
    for (int i = 1000; i >= 0; --i)
    {
        printf("B:%04d\n", i);
        sleep(0.5);
    }
    pthread_exit(NULL);
}
int main()
{
    // 创建线程
    pthread_t tid_A, tid_B;
    int res1 = pthread_create(&tid_A, NULL, pthread_A, NULL);
    int res2 = pthread_create(&tid_B, NULL, pthread_B, NULL);
    if (res1 != 0)
    {
        printf("Create thread A failed\nerror code:%d\n", res1);
        return 0;
    }
    if (res2 != 0)
    {
        printf("Create thread B failed\nerror code:%d\n", res2);
        return 0;
    }
    // 回收线程
    pthread_join(tid_A, NULL);
    pthread_join(tid_B, NULL);
    return 0;
}
// 哲学家问题-死锁解法
#include <windows.h>
#include <cstdio>
#include <ctime>
#include <cstdlib>
using namespace std;
#define PHIL_CNT 5 // 哲学家数量
#define SLEEP_TIME 3000

bool exitflag = true;                              // 用于退出的标志
int phichops[PHIL_CNT];                            // 每个哲学家的筷子(0:思考,1:一只筷子,2:进餐)
CRITICAL_SECTION cse;                              // 临界区
HANDLE s[PHIL_CNT];                                // 每根筷子对应的信号量
char status[3][20] = {"思考", "一只筷子", "进餐"}; // 哲学家的三种状态

inline void beforedining(int i, int k)
{
    printf("哲学家%d拿起%d号筷子现有%d支筷子 ", i + 1, k + 1, ++phichops[i]);
    if (phichops[i] == 1)
        printf("不能进餐\n");
    else if (phichops[i] == 2)
        printf("开始进餐\n");
}

inline void afterdining(int i, int k)
{
    printf("哲学家%d放下%d号筷子\n", i + 1, k + 1);
    --phichops[i];
}

inline void dining(int i)
{
    printf("哲学家%d就餐\n", i + 1);
    printf("哲学家1:%s  哲学家2:%s  哲学家3:%s  哲学家4:%s 哲学家5号:%s\n\n",
           status[phichops[0]], status[phichops[1]], status[phichops[2]], status[phichops[3]], status[phichops[4]]);
    Sleep(SLEEP_TIME);
}

DWORD WINAPI philosopher(LPVOID lpPara)
{
    int i = int(lpPara);
    while (exitflag)
    {
        auto stop = rand() % 400 + 100; // 产生100-500ms的随机时长
        Sleep(stop);
        WaitForSingleObject(s[i], INFINITE); // 等待左侧筷子可用
        beforedining(i, i);
        WaitForSingleObject(s[(i + PHIL_CNT - 1) % PHIL_CNT], INFINITE); // 等待右侧筷子可用
        beforedining(i, (i + PHIL_CNT - 1) % PHIL_CNT);
        dining(i);
        ReleaseSemaphore(s[(i + PHIL_CNT - 1) % PHIL_CNT], 1, NULL); // 放下右侧筷子
        afterdining(i, (i + PHIL_CNT - 1) % PHIL_CNT);
        ReleaseSemaphore(s[i], 1, NULL); // 放下左侧筷子
        afterdining(i, i);
    }
    return 0;
}

int main()
{
    srand(unsigned(time(nullptr)));
    for (int i = 0; i < PHIL_CNT; ++i)
        s[i] = CreateSemaphore(NULL, 1, 1, NULL);
    HANDLE hThread[PHIL_CNT];
    DWORD phs[PHIL_CNT];
    for (int i = 0; i < PHIL_CNT; ++i)
    {
        hThread[i] = CreateThread(NULL, 0, philosopher, LPVOID(i), 0, &phs[i]);
        if (!hThread[i])
            return -1;
    }
    while (exitflag)
    {
        if (getchar())
            exitflag = false;
    }
    return 0;
}


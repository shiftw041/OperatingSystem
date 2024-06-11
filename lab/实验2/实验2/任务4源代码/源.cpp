// 生产者-消费者问题
#include <windows.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#define BUFFLEN 10      // 缓冲区长度
#define PRODUCER_CNT 2  // 2个输入线程
#define CONSUMER_CNT 3  // 3个输出线程
#define SLEEP_TIME 1000 // 每次输出后的休眠时间，毫秒

int productid[2] = {1000, 2000}; // 2个生产者的生产产品起始号，1000-1999，2000-2999
int consumeid;                   // 消费的产品号
int buf[BUFFLEN];                // 10个元素的缓冲区
int pidx = 0, cidx = 0;          // 分别记录生产的序号和消费的序号
int consumerid[3] = {1, 2, 3};   // 消费者编号
int producerid[2] = {0, 1};      // 生产者编号
bool exitflag = true;            // 用于退出的标志
HANDLE buffill, bufnull;         // 信号量
CRITICAL_SECTION cse;            // 临界区

void produce(int no);
void consume(int no);
DWORD WINAPI producer(LPVOID lpPara);
DWORD WINAPI consumer(LPVOID lpPara); // 函数声明

void produce(int no)
{
    auto produnctId = ++productid[no];
    cout << "生产产品:" << produnctId << endl;
    cout << "将产品放入缓冲区:" << pidx + 1 << endl;
    buf[pidx] = produnctId;
    cout << "缓冲区状态:";
    for (int i = 0; i < BUFFLEN; ++i)
    {
        if (buf[i] != 0)
            cout << i + 1 << ':' << buf[i] << ' ';
        else
            cout << i + 1 << ':' << "NULL ";
        if (i == pidx)
            cout << "<-生产 ";
    }
    cout << endl
         << endl;
    pidx = (pidx + 1) % BUFFLEN;
    Sleep(SLEEP_TIME);
}
void consume(int no)
{
    consumeid = buf[cidx];
    cout << "消费者" << consumerid[no] << "消费产品:" << consumeid << endl;
    cout << "消费产品缓冲区位置:" << cidx + 1 << endl;
    cout << "缓冲区状态:";
    for (int i = 0; i < BUFFLEN; ++i)
    {
        if (buf[i] != 0)
            cout << i + 1 << ':' << buf[i] << ' ';
        else
            cout << i + 1 << ':' << "NULL ";
        if (i == cidx)
            cout << "<-消费 ";
    }
    cout << endl
         << endl;
    buf[cidx] = 0;
    cidx = (cidx + 1) % BUFFLEN;
    Sleep(SLEEP_TIME);
}

DWORD WINAPI producer(LPVOID lpPara)
{
    int no = int(lpPara); // 生产者编号
    while (exitflag)
    {
        auto step = rand() % 900 + 100;         // 随机一个100ms-1s的间隔时间
        WaitForSingleObject(bufnull, INFINITE); // 要求缓冲区有空位
        // INFINITE:对象被触发信号后函数才会返回
        EnterCriticalSection(&cse); // 互斥
        Sleep(step);
        produce(no);
        LeaveCriticalSection(&cse);         // 离开
        ReleaseSemaphore(buffill, 1, NULL); // 数据+1
    }
    return 0;
}

DWORD WINAPI consumer(LPVOID lpPara)
{
    int no = int(lpPara); // 消费者编号
    while (exitflag)
    {
        auto step = rand() % 900 + 100;         // 随机一个100ms-1s的间隔时间
        WaitForSingleObject(buffill, INFINITE); // 要求缓冲区有产品
        EnterCriticalSection(&cse);
        Sleep(step);
        consume(no);
        LeaveCriticalSection(&cse);
        ReleaseSemaphore(bufnull, 1, NULL);
    }
    return 0;
}

int main()
{
    srand(unsigned(time(nullptr)));
    // 创建信号量和临界区
    buffill = CreateSemaphore(NULL, 0, BUFFLEN, NULL);       // 缓冲区数据个数
    bufnull = CreateSemaphore(NULL, BUFFLEN, BUFFLEN, NULL); // 缓冲区空位个数
    InitializeCriticalSection(&cse);
    HANDLE hThread[PRODUCER_CNT + CONSUMER_CNT];
    DWORD producers[PRODUCER_CNT], consumers[CONSUMER_CNT];
    // 创建生产者线程
    for (int i = 0; i < PRODUCER_CNT; ++i)
    {
        hThread[i] = CreateThread(NULL, 0, producer, LPVOID(i), 0, &producers[i]);
        if (!hThread[i])
            return -1;
    }
    // 创建消费者线程
    for (int i = 0; i < CONSUMER_CNT; ++i)
    {
        hThread[PRODUCER_CNT + i] = CreateThread(NULL, 0, consumer, LPVOID(i), 0, &consumers[i]);
        if (!hThread[PRODUCER_CNT + i])
            return -1;
    }
    // 输入任意字符终止
    while (exitflag)
    {
        if (getchar())
            exitflag = false;
    }
    return 0;
}

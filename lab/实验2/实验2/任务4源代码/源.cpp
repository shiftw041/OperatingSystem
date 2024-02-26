// ������-����������
#include <windows.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;
#define BUFFLEN 10      // ����������
#define PRODUCER_CNT 2  // 2�������߳�
#define CONSUMER_CNT 3  // 3������߳�
#define SLEEP_TIME 1000 // ÿ������������ʱ�䣬����

int productid[2] = {1000, 2000}; // 2�������ߵ�������Ʒ��ʼ�ţ�1000-1999��2000-2999
int consumeid;                   // ���ѵĲ�Ʒ��
int buf[BUFFLEN];                // 10��Ԫ�صĻ�����
int pidx = 0, cidx = 0;          // �ֱ��¼��������ź����ѵ����
int consumerid[3] = {1, 2, 3};   // �����߱��
int producerid[2] = {0, 1};      // �����߱��
bool exitflag = true;            // �����˳��ı�־
HANDLE buffill, bufnull;         // �ź���
CRITICAL_SECTION cse;            // �ٽ���

void produce(int no);
void consume(int no);
DWORD WINAPI producer(LPVOID lpPara);
DWORD WINAPI consumer(LPVOID lpPara); // ��������

void produce(int no)
{
    auto produnctId = ++productid[no];
    cout << "������Ʒ:" << produnctId << endl;
    cout << "����Ʒ���뻺����:" << pidx + 1 << endl;
    buf[pidx] = produnctId;
    cout << "������״̬:";
    for (int i = 0; i < BUFFLEN; ++i)
    {
        if (buf[i] != 0)
            cout << i + 1 << ':' << buf[i] << ' ';
        else
            cout << i + 1 << ':' << "NULL ";
        if (i == pidx)
            cout << "<-���� ";
    }
    cout << endl
         << endl;
    pidx = (pidx + 1) % BUFFLEN;
    Sleep(SLEEP_TIME);
}
void consume(int no)
{
    consumeid = buf[cidx];
    cout << "������" << consumerid[no] << "���Ѳ�Ʒ:" << consumeid << endl;
    cout << "���Ѳ�Ʒ������λ��:" << cidx + 1 << endl;
    cout << "������״̬:";
    for (int i = 0; i < BUFFLEN; ++i)
    {
        if (buf[i] != 0)
            cout << i + 1 << ':' << buf[i] << ' ';
        else
            cout << i + 1 << ':' << "NULL ";
        if (i == cidx)
            cout << "<-���� ";
    }
    cout << endl
         << endl;
    buf[cidx] = 0;
    cidx = (cidx + 1) % BUFFLEN;
    Sleep(SLEEP_TIME);
}

DWORD WINAPI producer(LPVOID lpPara)
{
    int no = int(lpPara); // �����߱��
    while (exitflag)
    {
        auto step = rand() % 900 + 100;         // ���һ��100ms-1s�ļ��ʱ��
        WaitForSingleObject(bufnull, INFINITE); // Ҫ�󻺳����п�λ
        // INFINITE:���󱻴����źź����Ż᷵��
        EnterCriticalSection(&cse); // ����
        Sleep(step);
        produce(no);
        LeaveCriticalSection(&cse);         // �뿪
        ReleaseSemaphore(buffill, 1, NULL); // ����+1
    }
    return 0;
}

DWORD WINAPI consumer(LPVOID lpPara)
{
    int no = int(lpPara); // �����߱��
    while (exitflag)
    {
        auto step = rand() % 900 + 100;         // ���һ��100ms-1s�ļ��ʱ��
        WaitForSingleObject(buffill, INFINITE); // Ҫ�󻺳����в�Ʒ
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
    // �����ź������ٽ���
    buffill = CreateSemaphore(NULL, 0, BUFFLEN, NULL);       // ���������ݸ���
    bufnull = CreateSemaphore(NULL, BUFFLEN, BUFFLEN, NULL); // ��������λ����
    InitializeCriticalSection(&cse);
    HANDLE hThread[PRODUCER_CNT + CONSUMER_CNT];
    DWORD producers[PRODUCER_CNT], consumers[CONSUMER_CNT];
    // �����������߳�
    for (int i = 0; i < PRODUCER_CNT; ++i)
    {
        hThread[i] = CreateThread(NULL, 0, producer, LPVOID(i), 0, &producers[i]);
        if (!hThread[i])
            return -1;
    }
    // �����������߳�
    for (int i = 0; i < CONSUMER_CNT; ++i)
    {
        hThread[PRODUCER_CNT + i] = CreateThread(NULL, 0, consumer, LPVOID(i), 0, &consumers[i]);
        if (!hThread[PRODUCER_CNT + i])
            return -1;
    }
    // ���������ַ���ֹ
    while (exitflag)
    {
        if (getchar())
            exitflag = false;
    }
    return 0;
}

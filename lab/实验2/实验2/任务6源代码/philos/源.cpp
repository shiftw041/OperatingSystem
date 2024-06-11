// ��ѧ������-�����ⷨ
#include <windows.h>
#include <cstdio>
#include <ctime>
#include <cstdlib>
using namespace std;
#define PHIL_CNT 5 // ��ѧ������
#define SLEEP_TIME 3000

bool exitflag = true;                              // �����˳��ı�־
int phichops[PHIL_CNT];                            // ÿ����ѧ�ҵĿ���(0:˼��,1:һֻ����,2:����)
CRITICAL_SECTION cse;                              // �ٽ���
HANDLE s[PHIL_CNT];                                // ÿ�����Ӷ�Ӧ���ź���
char status[3][20] = {"˼��", "һֻ����", "����"}; // ��ѧ�ҵ�����״̬

inline void beforedining(int i, int k)
{
    printf("��ѧ��%d����%d�ſ�������%d֧���� ", i + 1, k + 1, ++phichops[i]);
    if (phichops[i] == 1)
        printf("���ܽ���\n");
    else if (phichops[i] == 2)
        printf("��ʼ����\n");
}

inline void afterdining(int i, int k)
{
    printf("��ѧ��%d����%d�ſ���\n", i + 1, k + 1);
    --phichops[i];
}

inline void dining(int i)
{
    printf("��ѧ��%d�Ͳ�\n", i + 1);
    printf("��ѧ��1:%s  ��ѧ��2:%s  ��ѧ��3:%s  ��ѧ��4:%s ��ѧ��5��:%s\n\n",
           status[phichops[0]], status[phichops[1]], status[phichops[2]], status[phichops[3]], status[phichops[4]]);
    Sleep(SLEEP_TIME);
}

DWORD WINAPI philosopher(LPVOID lpPara)
{
    int i = int(lpPara);
    while (exitflag)
    {
        auto stop = rand() % 400 + 100; // ����100-500ms�����ʱ��
        Sleep(stop);
        WaitForSingleObject(s[i], INFINITE); // �ȴ������ӿ���
        beforedining(i, i);
        WaitForSingleObject(s[(i + PHIL_CNT - 1) % PHIL_CNT], INFINITE); // �ȴ��Ҳ���ӿ���
        beforedining(i, (i + PHIL_CNT - 1) % PHIL_CNT);
        dining(i);
        ReleaseSemaphore(s[(i + PHIL_CNT - 1) % PHIL_CNT], 1, NULL); // �����Ҳ����
        afterdining(i, (i + PHIL_CNT - 1) % PHIL_CNT);
        ReleaseSemaphore(s[i], 1, NULL); // ����������
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


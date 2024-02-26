// LRU OPT��̭�㷨
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <stack>
#include <map>

using namespace std;
#define MAX_NUM 10000
const int pageSize = 10;      // ҳ���С
const int instrCnt = MAX_NUM; // ָ����Ŀ
const int pageFrameCnt = 3;   // ҳ����
int orderCnt;                 // ���ʴ���
int cnt = 0;                  // ��ǰҳ����
int LRUpageMissCnt = 0;       // ȱҳ��
int OPTpageMissCnt = 0;
int op;                                // ѡ��ҳ�滻�㷨
int processArr[instrCnt];              // ��������
int pageFrame[pageFrameCnt][pageSize]; // ҳ������
int orderArr[MAX_NUM];                 // ָ���������
int pageIdx[pageFrameCnt];             // ҳ���д��ҳ��
int arraykind;                         // �����������ͣ�0Ϊ�����1Ϊ˳��2Ϊѭ��

// �����������
inline void showOrderArray()
{
    for (int i = 0, j = 0; i < orderCnt; ++i, ++j)
    {
        printf("[%04d]%04d\t", orderArr[i] / pageSize, orderArr[i]);
        if ((j + 1) % 5 == 0)
            putchar('\n');
    }
    putchar('\n');
}
// ��ʼ������
inline void initProcess()
{
    for (int i = 0; i < instrCnt; ++i)
    {
        processArr[i] = rand() % 1000;
    }
}

// ��ҳ��ΪpageNoҳ������ݸ��Ƶ���pfIdx��ҳ����
inline void copyPage(int pfIdx, int pageNo)
{
    // ��������
    memcpy(pageFrame[pfIdx],processArr + pageNo * pageSize, pageSize * sizeof(int));
    pageIdx[pfIdx] = pageNo; // ��¼ҳ��
}

// ��ʼ����������
void initInstrOrder(int kind, int size)
{
    orderCnt = size;
    if (kind == 1)
    {
        for (int i = 0; i < orderCnt; ++i)
        {
            orderArr[i] = rand() % size;
        }
    }
    else
    {
        for (int i = 0; i < orderCnt; ++i)
        {
            orderArr[i] = i;
        }
    }
    return;
}

// ���ҳ��״̬
void showPageFrame()
{
    cout << "pageframe situation:\n";
    int i;
    for (i = 0; i < cnt; ++i)
    {
        cout << "pageframe:" << i + 1 << "  page:" << pageIdx[i] << "  content:";
        for (int j = 0; j < pageSize; ++j)
        {
            cout << pageFrame[i][j] << ' ';
        }
        putchar('\n');
    }
    for (; i < pageFrameCnt; ++i)
    {
        cout << "pageframe:" << i + 1 << " NULL\n";
    }
    putchar('\n');
}

// LRU
void LRU()
{
    int timer[pageFrameCnt];
    memset(timer, 0, sizeof(timer));
    for (int i = 0; i < orderCnt; ++i)
    {
        auto pageNo = orderArr[i] / pageSize; // ҳ��
        auto offset = orderArr[i] % pageSize; // ҳ��ƫ��
        // printf("wait to visit: %04d pageID:%04d value:%04d\n", orderArr[i], pageNo, processArr[orderArr[i]]);
        int j;
        // ����
        for (j = 0; j < cnt; ++j)
        {
            if (pageIdx[j] == pageNo)
            {
                // printf("Hit!! %04d\n", pageFrame[j][offset]);
                timer[j] = 0;
                break;
            }
        }
        // δ����
        if (j == cnt)
        {
            // printf("Miss!! ");
            ++LRUpageMissCnt; // ȱҳ����+1
            // ��ҳ����ȫռ��
            if (cnt == pageFrameCnt)
            {
                auto maxT = 0;
                // �ҵ�δʹ��ʱ�����ҳ�������̭
                for (int k = 0; k < pageFrameCnt; ++k)
                {
                    if (timer[k] > timer[maxT])
                        maxT = k;
                }
                copyPage(maxT, pageNo);
                timer[maxT] = 0;
                // printf("%04d\n",pageFrame[maxT][offset]);
            }
            // ҳ��δȫ��ռ����ֱ�ӽ�ҳ���Ƶ���ҳ��
            else
            {
                copyPage(cnt, pageNo);
                // printf("%04d\n", pageFrame[cnt][offset]);
                ++cnt;
            }
        }
        for (int j = 0; j < cnt; ++j)
            ++timer[j];
        // showPageFrame();
    }
    cout << "visit times:" << orderCnt << " miss times:" << LRUpageMissCnt
         << " LRU miss rate:" << float(LRUpageMissCnt) / orderCnt * 100 << "%" << endl;
    return;
}

// OPT
void OPT()
{
    // ������Ϣ��: ����Ϊ�����ʵ�ҳ��,��ֵΪһ�����и�ҳ�ŷ��ʴ���Ķ�ջ
    map<int, stack<int>> ms;
    // ���������������
    for (int i = orderCnt - 1; i >= 0; --i)
    {
        auto pageNo = orderArr[i] / pageSize; // ҳ��
        // ����ҳδ������
        if (ms.count(pageNo) == 0)
        {
            stack<int> tmp; // ������ջ
            tmp.push(i);    // �ڶ�ջ����ӷ��ʴ���
            ms.insert(pair<int, stack<int>>(pageNo, tmp));
        }
        else
        {
            ms.at(pageNo).push(i); // �ڶ�ջ����ӷ��ʴ���
        }
    }
    // ˳��ִ��
    for (int i = 0; i < orderCnt; ++i)
    {
        auto pageNo = orderArr[i] / pageSize; // ҳ��
        auto offset = orderArr[i] % pageSize; // ҳ��ƫ��
        // printf("wait to visit: %04d pageID:%04d value:%04d\n", orderArr[i], pageNo, processArr[orderArr[i]]);
        // ÿ��ִ���꽫��ҳջ���Ĵ�����Ϣ��ջ
        if (ms.at(pageNo).size())
            ms.at(pageNo).pop();
        int j;
        // ����ҳ��������
        for (j = 0; j < cnt; ++j)
        {
            if (pageIdx[j] == pageNo)
            {
                // printf("Hit!! %04d\n", pageFrame[j][offset]);
                break;
            }
        }
        // ��δ����
        if (j == cnt)
        {
            // cout << "Miss!! ";
            ++OPTpageMissCnt; // ȱҳ����+1
            // ��ҳ����ȫռ��
            if (cnt == pageFrameCnt)
            {
                auto maxT = 0;
                // ����ҳ����ݴ�����ϢѰ�Ҳ�����Ҫ����Զ�Ľ������õ�ҳ��
                for (int k = 0; k < pageFrameCnt; ++k)
                {
                    if (ms.at(pageIdx[k]).size() == 0)
                    {
                        maxT = k;
                        break;
                    }
                    else if (ms.at(pageIdx[k]).top() > ms.at(pageIdx[maxT]).top())
                    {
                        maxT = k;
                    }
                }
                // ��̭ҳ�渴��������
                copyPage(maxT, pageNo);
                // printf("%04d\n",pageFrame[maxT][offset]);
            }
            // ҳ��δȫ��ռ����ֱ�ӽ�ҳ���Ƶ���ҳ��
            else
            {
                copyPage(cnt, pageNo);
                // printf("%04d\n", pageFrame[cnt][offset]);
                ++cnt;
            }
        }
        // ���ÿ�ε�ҳ����Ϣ
        // showPageFrame();
    }
    cout << "visit times:" << orderCnt << " miss times:" << OPTpageMissCnt
         << " OPT miss rate:" << float(OPTpageMissCnt) / orderCnt * 100 << "%" << endl;
    return;
}

int main()
{
    srand(time(nullptr));
    printf("array size:\n");
    cin >> orderCnt;
    while (1)
    {
        initProcess();
        printf("select array kind:\n1.Random\n2.Sequence\n");
        cin >> arraykind;
        initInstrOrder(arraykind, orderCnt);
        // showOrderArray();
        printf("1.OPT\n2.LRU\n3.quit\n");
        cin >> op;
        if (op == 1)
            OPTpageMissCnt = 0, OPT();
        else if (op == 2)
            LRUpageMissCnt = 0, LRU();
        else
            break;
    }
    system("pause");
    return 0;
}
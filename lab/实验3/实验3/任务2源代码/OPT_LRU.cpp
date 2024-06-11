// LRU OPT淘汰算法
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <stack>
#include <map>

using namespace std;
#define MAX_NUM 10000
const int pageSize = 10;      // 页面大小
const int instrCnt = MAX_NUM; // 指令数目
const int pageFrameCnt = 3;   // 页框数
int orderCnt;                 // 访问次数
int cnt = 0;                  // 当前页框数
int LRUpageMissCnt = 0;       // 缺页数
int OPTpageMissCnt = 0;
int op;                                // 选择页替换算法
int processArr[instrCnt];              // 进程数组
int pageFrame[pageFrameCnt][pageSize]; // 页框数组
int orderArr[MAX_NUM];                 // 指令访问序列
int pageIdx[pageFrameCnt];             // 页框中存的页号
int arraykind;                         // 访问数组类型，0为随机，1为顺序，2为循环

// 输出访问序列
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
// 初始化进程
inline void initProcess()
{
    for (int i = 0; i < instrCnt; ++i)
    {
        processArr[i] = rand() % 1000;
    }
}

// 将页号为pageNo页面的数据复制到第pfIdx的页框中
inline void copyPage(int pfIdx, int pageNo)
{
    // 复制数据
    memcpy(pageFrame[pfIdx],processArr + pageNo * pageSize, pageSize * sizeof(int));
    pageIdx[pfIdx] = pageNo; // 记录页号
}

// 初始化访问序列
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

// 输出页框状态
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
        auto pageNo = orderArr[i] / pageSize; // 页号
        auto offset = orderArr[i] % pageSize; // 页内偏移
        // printf("wait to visit: %04d pageID:%04d value:%04d\n", orderArr[i], pageNo, processArr[orderArr[i]]);
        int j;
        // 命中
        for (j = 0; j < cnt; ++j)
        {
            if (pageIdx[j] == pageNo)
            {
                // printf("Hit!! %04d\n", pageFrame[j][offset]);
                timer[j] = 0;
                break;
            }
        }
        // 未命中
        if (j == cnt)
        {
            // printf("Miss!! ");
            ++LRUpageMissCnt; // 缺页次数+1
            // 若页框已全占满
            if (cnt == pageFrameCnt)
            {
                auto maxT = 0;
                // 找到未使用时间最长的页框进行淘汰
                for (int k = 0; k < pageFrameCnt; ++k)
                {
                    if (timer[k] > timer[maxT])
                        maxT = k;
                }
                copyPage(maxT, pageNo);
                timer[maxT] = 0;
                // printf("%04d\n",pageFrame[maxT][offset]);
            }
            // 页框未全部占满则直接将页复制到空页框
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
    // 待用信息表: 键名为待访问的页号,键值为一个存有该页号访问次序的堆栈
    map<int, stack<int>> ms;
    // 逆序变量访问序列
    for (int i = orderCnt - 1; i >= 0; --i)
    {
        auto pageNo = orderArr[i] / pageSize; // 页号
        // 若该页未被访问
        if (ms.count(pageNo) == 0)
        {
            stack<int> tmp; // 创建堆栈
            tmp.push(i);    // 在堆栈中添加访问次序
            ms.insert(pair<int, stack<int>>(pageNo, tmp));
        }
        else
        {
            ms.at(pageNo).push(i); // 在堆栈中添加访问次序
        }
    }
    // 顺序执行
    for (int i = 0; i < orderCnt; ++i)
    {
        auto pageNo = orderArr[i] / pageSize; // 页号
        auto offset = orderArr[i] % pageSize; // 页内偏移
        // printf("wait to visit: %04d pageID:%04d value:%04d\n", orderArr[i], pageNo, processArr[orderArr[i]]);
        // 每次执行完将该页栈顶的待用信息出栈
        if (ms.at(pageNo).size())
            ms.at(pageNo).pop();
        int j;
        // 遍历页框若命中
        for (j = 0; j < cnt; ++j)
        {
            if (pageIdx[j] == pageNo)
            {
                // printf("Hit!! %04d\n", pageFrame[j][offset]);
                break;
            }
        }
        // 若未命中
        if (j == cnt)
        {
            // cout << "Miss!! ";
            ++OPTpageMissCnt; // 缺页次数+1
            // 若页框已全占满
            if (cnt == pageFrameCnt)
            {
                auto maxT = 0;
                // 遍历页框根据待用信息寻找不在需要或最远的将来才用的页面
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
                // 淘汰页面复制新数据
                copyPage(maxT, pageNo);
                // printf("%04d\n",pageFrame[maxT][offset]);
            }
            // 页框未全部占满则直接将页复制到空页框
            else
            {
                copyPage(cnt, pageNo);
                // printf("%04d\n", pageFrame[cnt][offset]);
                ++cnt;
            }
        }
        // 输出每次的页框信息
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
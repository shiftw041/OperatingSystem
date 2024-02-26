#include <iostream>
#include <ctime>
#include <Windows.h>
using namespace std;
int times = 1;
const int row = 2048;
const int col = 1024;
int myArray[row * 10][col * 10];
int main()
{
    cout << "PID is:" << GetCurrentProcessId() << endl;
    int x, y;
    while (1)
    {
        cout << "1.column first\n2.row first\n3.quit\n";
        cin >> x;
        if (x == 3)
            return 0;
        cout << "1.big array(20480*10240)\n2.small array(2048*1024)\n";
        cin >> y;
        if (y == 1)
            times = 10;
        else
            times = 1;
        if (x == 1)
        { // 先列后行遍历数组
            clock_t s = clock();
            for (int i = 0; i < col * times; ++i)
            {
                for (int j = 0; j < row * times; ++j)
                {
                    myArray[j][i] = 0;
                }
            }
            clock_t e = clock();
            cout << "cost time:" << (double)(e - s) / CLOCKS_PER_SEC << "s\n";
        }
        else if (x == 2)
        { // 先行后列遍历数组
            clock_t s = clock();
            for (int i = 0; i < row * times; ++i)
            {
                for (int j = 0; j < col * times; ++j)
                {
                    myArray[i][j] = 0;
                }
            }
            clock_t e = clock();
            cout << "cost time:" << (double)(e - s) / CLOCKS_PER_SEC << "s\n";
        }
        system("pause");
    }
    return 0;
}

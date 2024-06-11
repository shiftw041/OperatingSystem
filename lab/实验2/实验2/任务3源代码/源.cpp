#include <graphics.h>
#include <stdio.h>
#include <Windows.h>
#include <math.h>
#pragma warning(disable : 6387)
#pragma warning(disable : 4244)
#define pi 3.1415926535897932

DWORD WINAPI drawSquare(LPVOID)
{
    // 分为720个点 180*4
    for (int i = 0; i < 180; i++)
    {
        putpixel(50 + i, 50, YELLOW);
        Sleep(5);
    }
    for (int i = 0; i < 180; i++)
    {
        putpixel(50 + 180, 50 + i, YELLOW);
        Sleep(5);
    }
    for (int i = 0; i < 180; i++)
    {
        putpixel(50 + 180 - i, 50 + 180, YELLOW);
        Sleep(5);
    }
    for (int i = 0; i < 180; i++)
    {
        putpixel(50, 50 + 180 - i, YELLOW);
        Sleep(5);
    }
    return 0;
}

DWORD WINAPI drawCircle(LPVOID)
{
    // 同样分为720个点
    for (int i = 0; i < 720; i++)
    {
        putpixel(350 + 100 * cos(-pi / 2 + (double)((i * pi) / 360)), 140 + 100 * sin(-pi / 2 + (double)((i * pi) / 360)), GREEN);
        Sleep(5);
    }
    return 0;
}
int main()
{
    // 初始化图形模式
    initgraph(480, 360);
    HANDLE square, circle;
    DWORD threadID; // 记录线程ID

    if ((square = CreateThread(NULL, 0, drawSquare, 0, 0, &threadID)) == NULL)
        printf("正方形线程创建失败!");
    if ((circle = CreateThread(NULL, 0, drawCircle, 0, 0, &threadID)) == NULL)
        printf("圆线程创建失败!");
    // 等待所有线程结束
    WaitForSingleObject(square, INFINITE);
    WaitForSingleObject(circle, INFINITE);
    CloseHandle(square);
    CloseHandle(circle);
    return 0;
}



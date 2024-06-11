#include <graphics.h>
#include <stdio.h>
#include <Windows.h>
#include <math.h>
#pragma warning(disable : 6387)
#pragma warning(disable : 4244)
#define pi 3.1415926535897932

DWORD WINAPI drawSquare(LPVOID)
{
    // ��Ϊ720���� 180*4
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
    // ͬ����Ϊ720����
    for (int i = 0; i < 720; i++)
    {
        putpixel(350 + 100 * cos(-pi / 2 + (double)((i * pi) / 360)), 140 + 100 * sin(-pi / 2 + (double)((i * pi) / 360)), GREEN);
        Sleep(5);
    }
    return 0;
}
int main()
{
    // ��ʼ��ͼ��ģʽ
    initgraph(480, 360);
    HANDLE square, circle;
    DWORD threadID; // ��¼�߳�ID

    if ((square = CreateThread(NULL, 0, drawSquare, 0, 0, &threadID)) == NULL)
        printf("�������̴߳���ʧ��!");
    if ((circle = CreateThread(NULL, 0, drawCircle, 0, 0, &threadID)) == NULL)
        printf("Բ�̴߳���ʧ��!");
    // �ȴ������߳̽���
    WaitForSingleObject(square, INFINITE);
    WaitForSingleObject(circle, INFINITE);
    CloseHandle(square);
    CloseHandle(circle);
    return 0;
}



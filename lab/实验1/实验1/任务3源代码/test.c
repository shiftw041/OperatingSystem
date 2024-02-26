#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/kernel.h>
int main()
{
    int nRet1 = syscall(548, 20, 18); // nRet1 = 38,第一个参数是系统调用号
    printf("add_result=%d\n", nRet1);
    nRet1 = syscall(549, 20, 18, 4); // nRet1=20
    printf("max_result=%d", nRet1);
    return 0;
}
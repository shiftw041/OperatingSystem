## 运行环境配置
Bochs x86-64 emulator
NASM version 2.14.02
## 源项目文件
os文件夹存储整个源项目文件
* freedos.img是官方提供的dos映像
* pmtest.img是已格式化的空白盘
* bochsrc.txt是bochs虚拟机的配置文件
* makefile是一键更新和运行程序的命令文件
* os.asm是编写的主要代码，os.com是编译生成的文件
* pm.inc存放宏定义，包括封装的宏函数
* lib.inc存放通用函数，如显示字符串、初始化芯片、清屏操作等等

## 如何测试运行
配置好nasm和bochs环境，打开终端切换到os文件夹，输入make回车即可启动虚拟机测试运行
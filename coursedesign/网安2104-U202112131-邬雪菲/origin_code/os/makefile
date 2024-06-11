# 在make之前目录中需要准备好freedos.img和格式化的空白盘pmtest.img
all: com image bochs
# 编译汇编代码
com:
	nasm os.asm -o os.com
# 挂载b盘并写入文件
image:
	- sudo rm -rf /mnt/floppyb/*
	mkdir -p /mnt/floppyb
	- sudo umount /mnt/floppyb
	sudo mount -o loop pmtest.img /mnt/floppyb
	sudo cp os.com /mnt/floppyb/os.com
# 启动bochs
bochs:
	bochs -f bochsrc.txt
# 清空
clean:
	sudo rm -rf /mnt/floppy/*
	- sudo umount /mnt/floppyb
.PHONY:
	all

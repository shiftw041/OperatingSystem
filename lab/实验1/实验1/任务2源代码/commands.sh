#check kernel
dpkg --list | grep linux-image
#install kernel original code
tar xf linux-5.8.4.tar.xz 
sudo mv linux-5.8.4 /usr/src/
sudo rm linux-5.8.4.tar.xz
#install software
sudo apt-get install gcc gdb bison flex libncurses5-dev libssl-dev libidn11 build-essential
#config kernel
sudo make mrproper 
sudo make clean
sudo make menuconfig
#change config
sudo pluma .config
#make
sudo make -j4
#install module
sudo make modules
sudo make modules_install
sudo make install
#update grub
sudo update-initramfs -c -k 5.8.4
sudo update-grub2
#go in new kernel
reboot
#check kernel
uname -a
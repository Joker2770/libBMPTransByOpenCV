// 先在系统内安装好opencv-2.4.13.6（过高的版本此项目可能编译不过）
// wget https://github.com/opencv/opencv/archive/2.4.13.6.tar.gz
// tar -zxvf 2.4.13.6.tar.gz
// cd 2.4.13.6   #切换到CMakeList.txt目录
// mkdir build
// cd build
// cmake ..
// make
// sudo make install #一般是安装到/usr/local/lib

mkdir build
cd build
cmake ..
make


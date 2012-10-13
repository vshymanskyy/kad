dir=$(pwd)
mkdir $dir/build
#mkdir $dir/build/linux-amd64; cd $dir/build/linux-amd64; cmake ../..; make
mkdir $dir/build/linux-armhf; cd $dir/build/linux-armhf; cmake -DCMAKE_TOOLCHAIN_FILE=../../linux-armhf.cmake ../..; make
#mkdir $dir/build/win32-x86; cd $dir/build/win32-x86; cmake -DCMAKE_TOOLCHAIN_FILE=../../Win32.cmake ../..; make
#mkdir $dir/build/win32-x64; cd $dir/build/win32-x64; cmake -DCMAKE_TOOLCHAIN_FILE=../../Win64.cmake ../..; make
#mkdir $dir/build/linux-i386; cd $dir/build/linux-i386; cmake -DCMAKE_TOOLCHAIN_FILE=../../Win32.cmake ../..; make

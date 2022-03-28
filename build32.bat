@echo off
rmdir /Q /S build
mkdir build
cd build

cmake -G "Visual Studio 16 2019" -A Win32 -S .. -B "build32" -DSMTG_CREATE_VST2_VERSION=ON -DVST3_SDK_ROOT=%VST3_SDK_ROOT_32BIT%
cmake --build build32 --config Release

rename build32\VST3\Release\__PLUGIN_NAME__.vst3\Contents\x86-win\__PLUGIN_NAME__.vst3 __PLUGIN_NAME__.dll

cd ..

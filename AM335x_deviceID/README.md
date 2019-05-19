# beaglebone

This is a program that gets the AM335x SoC silicon revision on your beagle bone
black. This assumes that you have installed PROCESSOR-SDK-LINUX-AM335X revision
05.03.00.07. 

Assumption: SDK path is /opt/ti/ti-processor-sdk-linux-am335x-evm-05.03.00.07.

./build.sh produces an executable AM335x_deviceID that can be executed on the
beaglebone black. This program should be executed with super user previlages.

steps to run:

1. ./build.sh
2. scp AM335x_deviceID debian@beaglebone:
3. ssh debian@beaglebone
4. sudo ./AM335x_deviceID

# OSES PROJECT - Group 4

Clone this repository using the `--recurse-submodules` option to obtain the FreeRTOS source code required to run the application.

Once the repository is cloned, the first thing to do is to build Qemu. To do so, follow these steps, from the project root folder:

```
cd qemu
mkdir build
cd build
sudo ../configure --target-list=arm-softmmu
sudo make -j8
sudo make install
```

Then, go to the `NXP_Startup` folder:

```
cd ../../NXP_Startup
```

and run the following command to build the FreeRTOS application:

```
make
```

and the following one to run it:

```
make run
```

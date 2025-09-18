<h1 align="center" style="border-bottom: none">
    <div>
    OS project 2025, Group 4 
        </a>
        <br>
    </div>
</h1>


# Emulation of the NXP S32K3X8EVB-Q289 board on Qemu

The content of this repository is a modified version of Qemu that includes the files needed to emulate the NXP S32K3X8EVB-Q289 board, so to be able to run software onto the board without the necessity of having the physical hardware.  

Inside the project are present:
- the S32K358 microcontroller
- a UART peripheral
- a CAN peripheral
- the files needed to import FreeRTOS on the board
- a demo project with personalized startup and linker


## Usage

The project run on Linux and can be imported from our git repository with the following command:

```bash
git clone --recurse-submodules https://baltig.polito.it/eos2024/group4.git
```

To run your code on the emulated board, first and foremost you have to compile Qemu. To do so we run first:

```bash
cd qemu/build
../configure --target-list=arm-softmmu
```
so to be on the build directory of Qemu (in case it's not present you can create it with the command makedir) and configure its Makefile to compile only the useful files.
Then run:

```bash
make
sudo make install
```

where the second command is necessary only if you want to access Qemu from outside its directory as we are about to do.

Then open another terminal on directory where its present the code to run (for our demo it's the folder demo), and simply run:

```bash
make all
make run
```

To debug the code are also already present the following commands:

```bash
make run_debug       # to run Qemu in debug mode 
make debug              # to open gdb on the running application

make run_monitor    # to run Qemu with its monitor open
make open_monitor  # to open the monitor of Qemu
```

## Description

To define the board we used Qemu framework, that defines the majority of the backend and makes a lot simpler to implement all the emulated hardware. The board is define as a hierarchy of structs, starting from the board to the microcontroller that includes all its components like memories, CPUs, irq controller,...
The code can be found in the the following files:

> * qemu/hw/arm/npx_s32k3.c 
> * qemu/hw/arm/npx_s32k3_board.c 
> * qemu/include/hw/arm/npx_s32k3.h

Note that the microcontroller is implemented as a device with three core, but two of the three are off on reset so that the boot is done on only one cpu.

The peripherals are implemented as separeted devices that after are connected to the board. More information on them can be found in the README_CAN, README_UART and in the following files with the code:

> * qemu/hw/char/uart.c 
> * qemu/include/hw/char/uart.h
> * qemu/hw/net/can/can.c
> * qemu/include/hw/net/can.h

## Roadmap

To continue the developement of the project, various improvement can be made:
- the Qemu bus can be substitute with a more feithful implementation of the bus present on the board;
- the microcontroller can be made as a real multiprocessor system by adding the code necessary to load a kernel also on the other two cores;
- better emulation of the real bheaviour of the cores can be emulated by making two of the cores run in a lockstep mode;
- other peripherals could be included, like a DMA, or timers;

## Authors

- Claudio Pio Perricone
- Gabriele Arcidiacono
- Matteo Ruggeri
- Stefano Galati


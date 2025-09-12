# Default target
all: build/main.elf

# Compile C sources
build/main.elf: main.c startup.c
	arm-none-eabi-gcc -c -mcpu=cortex-m7 -g -F -Wall -O0 main.c -o build/main.o
	arm-none-eabi-gcc -c -mcpu=cortex-m7 -g -F -Wall -O0 startup.c -o build/startup.o
	arm-none-eabi-ld build/startup.o build/main.o -T linker.ld -o build/main.elf

PHONY: run
run: 
	qemu-system-arm -M nxps32k3-board -kernel build/main.elf -s -S

# Default target
all: build/main.elf

# Compile C sources
build/main.elf: main.c startup.c
	arm-none-eabi-gcc -c -mcpu=cortex-m7 -g -F -Wall -O0 main.c -o build/main.o
	arm-none-eabi-gcc -c -mcpu=cortex-m7 -g -F -Wall -O0 startup.c -o build/startup.o
	arm-none-eabi-ld build/startup.o build/main.o -T linker.ld -o build/main.elf

PHONY: run, debug
run: 
	qemu-system-arm -M nxps32k3-board -kernel build/main.elf -s -S
debug:
	gdb-multiarch build/main.elf \
		-ex "target remote localhost:1234"
run-qemu:
	qemu-system-arm -M nxps32k3-board -kernel build/main.elf -d int,op,cpu,guest_errors,unimp,in_asm,out_asm,op_opt,exec,mmu,cpu_reset,page,nochain,strace,invalid_mem -D qemu.log


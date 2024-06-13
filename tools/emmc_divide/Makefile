.PHONY : all
all:
	${CROSS_COMPILE}gcc -fstack-protector-strong -s -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack emmc_divide.c -o emmc_divide
.PHONY : clean
clean:
	rm emmc_divide
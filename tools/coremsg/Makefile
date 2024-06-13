.PHONY : all
all:
	${CROSS_COMPILE}gcc -fstack-protector-strong -s -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack coremsg.c -o coremsg
.PHONY : clean
clean:
	rm coremsg

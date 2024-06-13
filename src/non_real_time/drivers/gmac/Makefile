include $(SDK_ROOT)/Makefile.cfg

obj-m := gmac_drv.o
gmac_drv-objs := gmac.o ethtool.o eth_rx.o eth_tx.o gmac_hw.o marvel_phy.o reltek_phy.o motor_comm_phy.o

MODULE_MAKE := $(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='gmac' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean

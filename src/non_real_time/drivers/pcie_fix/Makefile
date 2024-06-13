include $(SDK_ROOT)/Makefile.cfg

obj-m := pci_fix_drv.o
pci_fix_drv-objs := pci_fix_ker.o


MODULE_MAKE=$(PWD)
all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='pcie_fix' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean

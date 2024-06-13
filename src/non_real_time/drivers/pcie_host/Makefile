include $(SDK_ROOT)/Makefile.cfg

obj-m := pcie_hisi02_drv.o
pcie_hisi02_drv-objs := pcie_hisi02.o pcie_hisi_hip02.o pcie_framework.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='pcie_host' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean


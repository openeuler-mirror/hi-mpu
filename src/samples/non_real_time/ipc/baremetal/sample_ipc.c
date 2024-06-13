/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include "securec.h"
#include "openamp_module.h"
#include "sample_ipc_common.h"
#define HI_CORE3_SIZE  0x100000

static int copy_slave_bin(void *file_addr, void *sh_bin_addr, int bin_size, unsigned long load_address_slave, int ret)
{
    /* load clientos */
    unsigned long state_arg = 3;
    if (memcpy_s(sh_bin_addr, bin_size, file_addr, bin_size) != 0) {
        printf("Failed to copy the slave system\n");
        ioctl(ret, IOC_CLOSE_OPENAMP, &state_arg);
        close(ret);
        munmap(sh_bin_addr, bin_size);
        munmap(file_addr, bin_size);
        return -1;
    }

    /* unmap bin file, both from the Linux and shared memory */
    munmap(file_addr, bin_size);
    munmap(sh_bin_addr, bin_size);

    /* load cpu slave */
    int status;
    unsigned long boot_args[2];
    unsigned int cpu_slave_id = 3;
    boot_args[0] = cpu_slave_id;
    boot_args[1] = load_address_slave;
    status = ioctl(ret, IOC_CPUON, boot_args);
    if (status < 0) {
        printf("boot clientos failed\n");
        ioctl(ret, IOC_CLOSE_OPENAMP, &state_arg);
        close(ret);
        return -1;
    }
    close(ret);

    return 0;
}

static int repeat_poweron_core3(int ret, unsigned int cpu_slave_id)
{
    unsigned long state_arg = cpu_slave_id;
    int state_ret = ioctl(ret, IOC_AFFINITY_INFO, &state_arg);
    if (state_ret < 0) {
        printf("acquire cpu state failed\n");
        close(ret);
        return -1;
    }

    if (state_arg != CPU_STATE_OFF) {
        printf("cpu(%d) is already on.\n", cpu_slave_id);
        close(ret);
        return -1;
    }

    return 0;
}

static int load_slave_bin(char *path, char *slave_binaddr)
{
    unsigned long load_address_slave;
    load_address_slave = strtol(slave_binaddr, NULL, STR_TO_HEX);
    int bin_fd;
    struct stat buf;
    void *file_addr, *sh_bin_addr;
    int bin_size;
    unsigned int cpu_slave_id = 3;

    int ret = open(MCS_DEVICE_NAME, O_RDWR | O_SYNC);
    if (ret < 0) {
        printf("open failed, mcs:%d\n", ret);
        return ret;
    }

    unsigned int states = repeat_poweron_core3(ret, cpu_slave_id);
    if (states < 0) {
        return -1;
    }

    /* open clientos bin file from Linux file system */
    bin_fd = open(path, O_RDONLY);
    if (bin_fd < 0) {
        printf("open %s failed, bin_fd:%d\n", path, bin_fd);
        close(ret);
        return -1;
    }
    /* memory for loading clientos bin file */
    fstat(bin_fd, &buf);
    bin_size = PAGE_ALIGN(buf.st_size);

    sh_bin_addr = mmap(NULL, HI_CORE3_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, ret, load_address_slave);
    if (sh_bin_addr == MAP_FAILED) {
        printf("mmap reserved mem failed\n");
        close(ret);
        close(bin_fd);
        return -1;
    }

    unsigned long state_arg = cpu_slave_id;
    /* the address of bin file in Linux */
    file_addr = mmap(NULL, HI_CORE3_SIZE, PROT_READ, MAP_PRIVATE, bin_fd, 0);
    if (file_addr == MAP_FAILED) {
        printf("mmap failed: file_addr\n");
        ioctl(ret, IOC_CLOSE_OPENAMP, &state_arg);
        close(ret);
        close(bin_fd);
        munmap(sh_bin_addr, bin_size);
        return -1;
    }
    close(bin_fd);

    return copy_slave_bin(file_addr, sh_bin_addr, bin_size, load_address_slave, ret);
}

static int check_input_params_baremetal(char *cpu_id, char *target_binfile, char *target_binaddr,
    char *binfile_slave, char *binaddr_slave, char *isolate_cpus)
{
    int ret = -1;
    int slave_cpus = 2;
    int slave_cpu_index = 2;
    if (isolate_cpus == NULL || target_binfile == NULL || target_binaddr == NULL || cpu_id == NULL) {
        printf("input params is invaild!\r\n");
        return ret;
    }

    unsigned int isolate_cpu = strtol(isolate_cpus, NULL, STR_TO_DEC);
    if (isolate_cpu != slave_cpus || binfile_slave == NULL || binaddr_slave == NULL) {
        printf("input params is invaild!\r\n");
        return ret;
    }

    if (!(strtol(cpu_id, NULL, STR_TO_DEC) == slave_cpu_index)) {
        printf("input params cpu_id is invaild!\r\n");
        return ret;
    }

    return 0;
}

static int execute_slave_system(char *cpu_id, char *target_binfile, char *target_binaddr, char *binfile_slave,
    char *binaddr_slave, char *isolate_cpus)
{
    /* ctrl+c signal, do cleanup before program exit */
    signal(SIGINT, cleanup);
    int ret;
    ret = check_input_params_baremetal(cpu_id, target_binfile, target_binaddr,
        binfile_slave, binaddr_slave, isolate_cpus);
    if (ret) {
        return ret;
    }

    ret = load_slave_bin(binfile_slave, binaddr_slave);
    if (ret) {
        printf("load baremetal mcs_core3.bin failed\n");
        return ret;
    }

    ret = load_client_os(cpu_id, target_binfile, target_binaddr);
    if (ret) {
        printf("load baremetal mcs_core2.bin failed\n");
        return ret;
    }

    return ret;
}

int main(int argc, char **argv)
{
    int param_nums = 13;
    if (argc != param_nums) {
        printf("The number of input parameters is incorrect!\n");
        return 0;
    }
    
    int opt;
    char *cpu_id = NULL;
    char *target_binfile = NULL;
    char *target_binaddr = NULL;
    char *binfile_slave = NULL;
    char *binaddr_slave = NULL;
    char *isolate_cpus = NULL;

    while ((opt = getopt(argc, argv, "i:c:t:a:m:n:")) != -1) {
        switch (opt) {
            case 'i':
                isolate_cpus = optarg;
                break;
            case 'c':
                cpu_id = optarg;
                break;
            case 't':
                target_binfile = optarg;
                break;
            case 'a':
                target_binaddr = optarg;
                break;
            case 'm':
                binfile_slave = optarg;
                break;
            case 'n':
                binaddr_slave = optarg;
                break;
            case '?':
                printf("Unknown option: %c ", (char)optopt);
            default:
                break;
        }
    }

    return execute_slave_system(cpu_id, target_binfile, target_binaddr,
        binfile_slave, binaddr_slave, isolate_cpus);
}

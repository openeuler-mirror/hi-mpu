/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved
 *
 * SPDX-License-Identifier: MulanPSL-2.0
 */

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include "openamp_module.h"

volatile sig_atomic_t g_app_exiting = 0; // app is exiting

#define HI_CORE2_SIZE  0x100000

static struct client_os_inst client_os = {
    /* physical address start of shared device mem */
    .phy_shared_mem = 0x90000000,
    /* size of shared device mem */
    .shared_mem_size = 0x30000,
    .vring_size = VRING_SIZE,
    .vdev_status_size = VDEV_STATUS_SIZE,
};

static void cleanup(int sig)
{
    g_app_exiting = 1; // The cleanup operation is performed after the main loop exits.
}

static int load_slave_bin(char *path, char *slave_binaddr)
{
    unsigned long load_address_slave;
    load_address_slave = strtol(slave_binaddr, NULL, STR_TO_HEX);
    int bin_fd;
    struct stat buf;
    void *file_addr, *sh_bin_addr;
    int bin_size;
    void *sh_mem_addr;
    unsigned int cpu_slave_id = 3;

    int ret = open(MCS_DEVICE_NAME, O_RDWR | O_SYNC);
    if (ret < 0) {
        printf("open mcs failed :%d\n", ret);
        return ret;
    }

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

    sh_bin_addr = mmap(NULL, HI_CORE2_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, ret, load_address_slave);
    if (sh_bin_addr == MAP_FAILED) {
        printf("mmap reserved mem failed\n");
        close(ret);
        close(bin_fd);
        return -1;
    }
    
    /* the address of bin file in Linux */
    file_addr = mmap(NULL, HI_CORE2_SIZE, PROT_READ, MAP_PRIVATE, bin_fd, 0);
    if (file_addr == MAP_FAILED) {
        printf("mmap failed: file_addr\n");
        ioctl(ret, IOC_CLOSE_OPENAMP, &state_arg);
        close(ret);
        close(bin_fd);
        munmap(sh_bin_addr, bin_size);
        return -1;
    }
    close(bin_fd);

    /* load clientos */
    if (bin_size < buf.st_size) {
        printf("The file storage size is exceededr\n");
        ioctl(ret, IOC_CLOSE_OPENAMP, &state_arg);
        close(ret);
        munmap(file_addr, bin_size);
        munmap(sh_bin_addr, bin_size);
        return -1;
    }
    memcpy(sh_bin_addr, file_addr, buf.st_size);

    /* unmap bin file, both from the Linux and shared memory */
    munmap(file_addr, bin_size);
    munmap(sh_bin_addr, bin_size);

    /* load cpu slave*/
    int status;
    unsigned long boot_args[2];
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

static int endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
    char* print_chars = (char *)data;
    unsigned int i;
    for (i = 0; i < len; ++i) {
        printf("%c\n", print_chars[i]);
    }

    return 0;
}

static int rpmsg_app_master(struct client_os_inst *client)
{
    if (client == NULL) {
        printf("Please enter the correct client name:libck\n");
        return -1;
    }

    int register_endpoint_console = openamp_create_endpoint("console", endpoint_cb);
    if (register_endpoint_console < 0) {
        return -1;
    }

    int usleep_master = 1;
    int tick_count = 15000;
    while (!openamp_endpoint_ready("console")) {
        if (tick_count < 0) {
            printf("The %s endpoint isn't ready\n", "console");
            return -1;
        }
        usleep(usleep_master);
        tick_count--;
    }
    printf("The %s endpoint is ready\n", "console");

    void *buffer_addr;
    uint32_t buffer_len;
    char *send_message_chars = "aaaaaaaaaaaa\n";
    buffer_addr = get_zero_cpy_data_addr("console", &buffer_len);
    if ((int) buffer_len < strlen(send_message_chars)) {
        printf("Send more data than the maximum buffer\r\n");
        return -1;
    }

    /* Being of  write data for buffer */
    memcpy(buffer_addr, send_message_chars, strlen(send_message_chars));
    int ret = openamp_send_message("console", (void *)buffer_addr, strlen(send_message_chars));
    if (ret < 0) {
        printf("Failed to send data\r\n");
        return ret;
    }

    return 0;
}

static int check_input_params(char *cpu_id, char *target_binfile, char *target_binaddr,
                             char *binfile_slave, char *binaddr_slave, char *isolate_cpus)
{
    int ret = 1;
    if (isolate_cpus == NULL || target_binfile == NULL || target_binaddr == NULL || cpu_id == NULL) {
        printf("input params is invaild!\r\n");
        return ret;
    }

    unsigned int isolate_cpu = strtol(isolate_cpus, NULL, STR_TO_DEC);
    if (isolate_cpu != 2 || binfile_slave == NULL || binaddr_slave == NULL) {
        printf("input params is invaild!\r\n");
        return ret;
    }

    int slave_cpu_index = 2;
    if (!(strtol(cpu_id, NULL, STR_TO_DEC) == slave_cpu_index)) {
        printf("input params cpu_id is invaild!\r\n");
        return ret;
    }

    return 0;
}

static int load_client_os(char *cpu_id, char *target_binfile, char *target_binaddr, char *target_entry)
{
    int ret;
    client_os.cpu_id = strtol(cpu_id, NULL, STR_TO_DEC);
    client_os.load_address = strtol(target_binaddr, NULL, STR_TO_HEX);
    client_os.entry = target_entry ? strtol(target_entry, NULL, STR_TO_HEX) :
                        client_os.load_address;
    client_os.path = target_binfile;

    printf("cpu:%d, path:%s\n", client_os.cpu_id, client_os.path);

    ret = openamp_init(&client_os);
    if (ret) {
        printf("openamp init failed: %d\n", ret);
        return ret;
    }

    ret = rpmsg_app_master(&client_os);
    if (ret) {
        printf("rpmsg app master failed: %d\n", ret);
        openamp_deinit(&client_os);
        return ret;
    }

    int sleep_master = 200;
    while (g_app_exiting == 0) {
        usleep(sleep_master);
    }

    openamp_deinit(&client_os);

    return 0;
}

int main(int argc, char **argv)
{
    int param_nums = 13;
    if (argc != param_nums) {
        printf("The number of input parameters is incorrect!\n");
        return 0;
    }

    int ret;
    int opt;
    char *cpu_id = NULL;
    char *target_binfile = NULL;
    char *target_binaddr = NULL;
    char *target_entry = NULL;
    char *binfile_slave = NULL;
    char *binaddr_slave = NULL;
    char *isolate_cpus = NULL;

    /* ctrl+c signal, do cleanup before program exit */
    signal(SIGINT, cleanup);

    /* \todo: parameter check */
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

    ret = check_input_params(cpu_id, target_binfile, target_binaddr, binfile_slave, binaddr_slave, isolate_cpus);
    if (ret) {
        return ret;
    }

    ret = load_slave_bin(binfile_slave, binaddr_slave);
    if (ret) {
        return ret;
    }

    ret = load_client_os(cpu_id, target_binfile, target_binaddr, target_entry);
    if (ret) {
        return ret;
    }
    return 0;
}

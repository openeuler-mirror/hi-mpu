/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include "openamp_module.h"
#include "sample_ipc_common.h"

#define COUNTS_SENDS  2500
#define ENDPOINT_IPC   "console"
unsigned long long g_store_ticks[COUNTS_SENDS][2];
unsigned int g_count_send = 0;
unsigned int g_end_flag = COUNTS_SENDS;

volatile sig_atomic_t g_app_exiting = 0;

static struct client_os_inst client_os = {
    /* physical address start of shared device mem */
    .phy_shared_mem = 0x90000000,
    /* size of shared device mem */
    .shared_mem_size = 0x30000,
    .vring_size = VRING_SIZE,
    .vdev_status_size = VDEV_STATUS_SIZE,
};

void cleanup(int sig)
{
    g_app_exiting = 1; // The cleanup operation is performed after the main loop exits.
}

static void write_message_send(int data_len)
{
    unsigned long long start, end;
    asm volatile("mrs %0, cntvct_el0" : "=r" (start));
    void *buffer_addr;

    uint32_t buffer_len;
    buffer_addr = get_zero_cpy_data_addr(ENDPOINT_IPC, &buffer_len);
    if ((int) buffer_len < data_len) {
        printf("Send more data than the maximum buffer\r\n");
        return;
    }

    /* Being of  write data for buffer */
    if (memset_s(buffer_addr, buffer_len, 'a', data_len) != 0) {
        printf("Description Filling failed because the maximum value of the buffer exceeded\r\n");
        return;
    }
    asm volatile("mrs %0, cntvct_el0" : "=r" (end));

    /* End of write data for buffer */
    int ret = openamp_send_message(ENDPOINT_IPC, (void *)buffer_addr, data_len);
    if (ret < 0) {
        printf("Failed to send data\r\n");
        return;
    }

    if (g_count_send < g_end_flag) {
        g_store_ticks[g_count_send][0] = end - start;
    }

    g_count_send++;
    if (g_count_send == g_end_flag) {
        long sums_ticks = 0;
        for (int i = 1; i < g_end_flag; i++) {
            unsigned long long trip = g_store_ticks[i][1] - g_store_ticks[i - 1][1] - g_store_ticks[i - 1][0];
            sums_ticks += trip;
            printf("openeuler  i = %d, trip = %llu \n", i, trip);
        }
        printf("openeuler %d ticks average = %ld\n", data_len, sums_ticks / (COUNTS_SENDS - 1));
    }
    return;
}

static int pty_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
    unsigned long long start;
    if (g_count_send < g_end_flag) {
        asm volatile("mrs %0, cntvct_el0" : "=r" (start));
        g_store_ticks[g_count_send][1] = start;
    }
    int chars_len = 16;
    if (g_count_send < g_end_flag) {
        write_message_send(chars_len);
    }

    return 0;
}

static int rpmsg_app_master(struct client_os_inst *client)
{
    if (client == NULL) {
        printf("Please enter the correct client name\n");
        return -1;
    }

    int register_endpoint_console = openamp_create_pty_endpoint(ENDPOINT_IPC, pty_endpoint_cb);
    if (register_endpoint_console < 0) {
        return -1;
    }

    return 0;
}

int check_input_params(char *cpu_id, char *target_binfile, char *target_binaddr)
{
    int ret = -1;
    if (target_binfile == NULL || target_binaddr == NULL || cpu_id == NULL) {
        printf("input params is invaild!\r\n");
        return ret;
    }

    int cpu_index = 3;
    if (!(strtol(cpu_id, NULL, STR_TO_DEC) == cpu_index)) {
        printf("input params cpu_id is invaild!\r\n");
        return ret;
    }

    return 0;
}

int load_client_os(char *cpu_id, char *target_binfile, char *target_binaddr)
{
    int ret;
    client_os.cpu_id = strtol(cpu_id, NULL, STR_TO_DEC);
    client_os.load_address = strtol(target_binaddr, NULL, STR_TO_HEX);
    client_os.entry = client_os.load_address;
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
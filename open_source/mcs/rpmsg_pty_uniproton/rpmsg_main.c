/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved
 *
 * SPDX-License-Identifier: MulanPSL-2.0
 */

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#include "openamp_module.h"

volatile sig_atomic_t g_app_exiting = 0; // app is exiting

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

static int check_input_params(char *cpu_id, char *target_binfile, char *target_binaddr)
{
    int ret = 1;
    if (target_binfile == NULL || target_binaddr == NULL || cpu_id == NULL) {
        printf("input params is invaild!\r\n");
        return ret;
    }

    int slave_cpu_index = 3;
    if (!(strtol(cpu_id, NULL, STR_TO_DEC) == slave_cpu_index)) {
        printf("input params cpu_id is invaild!\r\n");
        return ret;
    }

    return 0;
}

int main(int argc, char **argv)
{
    int param_nums = 7;
    if (argc != param_nums) {
        printf("The number of input parameters is incorrect!\n");
        return 0;
    }

    int ret;
    int opt;
    char *cpu_id = NULL;
    char *target_binfile = NULL;
    char *target_binaddr = NULL;

    /* ctrl+c signal, do cleanup before program exit */
    signal(SIGINT, cleanup);

    /* \todo: parameter check */
    while ((opt = getopt(argc, argv, "c:t:a:")) != -1) {
        switch (opt) {
            case 'c':
                cpu_id = optarg;
                break;
            case 't':
                target_binfile = optarg;
                break;
            case 'a':
                target_binaddr = optarg;
                break;
            case '?':
                printf("Unknown option: %c ",(char)optopt);
            default:
                break;
        }
    }

    ret = check_input_params(cpu_id, target_binfile, target_binaddr);
    if (ret) {
        return ret;
    }

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

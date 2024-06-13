/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#include "openamp_module.h"
#include "sample_ipc_common.h"

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

    signal(SIGINT, cleanup);  /* ctrl+c signal, do cleanup before program exit */
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
                printf("Unknown option: %c ", (char)optopt);
            default:
                break;
        }
    }

    ret = check_input_params(cpu_id, target_binfile, target_binaddr);
    if (ret < 0) {
        return ret;
    }

    ret = load_client_os(cpu_id, target_binfile, target_binaddr);
    if (ret) {
        printf("Loading from core failed!\n");
        return ret;
    }

    return 0;
}

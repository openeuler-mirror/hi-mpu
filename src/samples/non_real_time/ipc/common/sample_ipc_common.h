/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#ifndef SAMPLE_IPC_COMMON_H
#define SAMPLE_IPC_COMMON_H

int check_input_params(char *cpu_id, char *target_binfile, char *target_binaddr);

int load_client_os(char *cpu_id, char *target_binfile, char *target_binaddr);

void cleanup(int sig);

#endif
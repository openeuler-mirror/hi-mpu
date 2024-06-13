/* 
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved
 *
 * SPDX-License-Identifier: MulanPSL-2.0
 */

#ifndef OPENAMP_PTY_H
#define OPENAMP_PTY_H

#if defined __cplusplus
extern "C" {
#endif

#define KEY_CTRL_D      4
#define KEY_ENTER_PTY   13

struct pty_ep_data {
    unsigned int ep_id; /* endpoint id */

    int fd_master;  /* pty master fd */

    pthread_t pty_thread; /* thread id */
};

struct openamp_data {
    pthread_t openamp_thread;
};

/* create pthread for pty */
void *pty_thread(void *arg);

extern struct openamp_data *g_openamp_data;

extern bool openamp_exiting; // openamp is exiting

#if defined __cplusplus
}
#endif

#endif  /* OPENAMP_PTY_H */
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved
 *
 * SPDX-License-Identifier: MulanPSL-2.0
 */

#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>

#include "openamp_module.h"

int open_pty(int *pfdm)
{
    int ret = -1;
    int fdm;
    char pts_name[20] = {0};

    /* Open the master side of the PTY */
    fdm = posix_openpt(O_RDWR | O_NOCTTY);
    if (fdm < 0) {
        printf("Error %d on posix_openpt()\n", errno);
        return  ret;
    }

    printf("pty master fd is :%d\n", fdm);

    ret = grantpt(fdm);
    if (ret != 0) {
        printf("Error %d on grantpt()\n", errno);
        return ret;
    }

    ret = unlockpt(fdm);
    if (ret != 0) {
        printf("Error %d on unlockpt()\n", errno);
        return ret;
    }

    /* Open the slave side of the PTY */
    ret = ptsname_r(fdm, pts_name, sizeof(pts_name));
    if (ret != 0) {
        printf("Error %d on ptsname_r()\n", errno);
        return ret;
    }

    printf("pls open %s to talk with client OS\n", pts_name);

    *pfdm = fdm;

    return 0;
}

void *pty_thread(void *arg)
{
    int ret;
    int fdm;
    unsigned char pty_chars;
    int index = 0;
    struct pty_ep_data *pty_ep;
    unsigned char shell_chars[128];

    pty_ep = (struct pty_ep_data *)arg;

    printf("pty_thread for %s is runnning\n", rpmsg_service_endpoint_name(pty_ep->ep_id));
    fdm = pty_ep->fd_master;

    /* wait endpoint bound */
    while(!rpmsg_service_endpoint_is_bound(pty_ep->ep_id));

    while (1) {
        ret = read(fdm, &pty_chars, 1);   /* get command from ptmx */
        if (ret <= 0) {
            printf("shell_user: get from ptmx failed: %d\n", ret);
            ret = -1;
            break;
        }

        if (pty_chars == KEY_CTRL_D) {  /* special key: ctrl+d */
            ret = 0;  /* exit this thread, the same as pthread_exit */
            break;
        }

        if (pty_chars == KEY_ENTER_PTY) {
            write(fdm, "\r\n", 2);
            shell_chars[index++] = '\r';
            shell_chars[index++] = '\n';
            ret = rpmsg_service_send(pty_ep->ep_id, (void *)shell_chars, index);
            if (ret < 0) {
                index = 0;
                break;
            }
            index = 0;
        }

        if (index > 124) {
            printf("Excessive data length\n");
            break;
        }

        shell_chars[index] = pty_chars;
        index++;
        write(fdm, &pty_chars, 1);
    }

    openamp_destroy_pty_endpoint(pty_ep);

    return (void *)((unsigned long)(ret));
}
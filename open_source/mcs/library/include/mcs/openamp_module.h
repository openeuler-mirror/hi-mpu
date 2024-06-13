/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved
 *
 * SPDX-License-Identifier: MulanPSL-2.0
 */

#ifndef OPENAMP_MODULE_H
#define OPENAMP_MODULE_H

#include <openamp/open_amp.h>

#include "mcs_common.h"
#include "remoteproc_module.h"
#include "virtio_module.h"
#include "rpmsg_endpoint.h"
#include "rpmsg_rpc_service.h"
#include "rpmsg_sys_service.h"
#include "openamp_pty.h"

#if defined __cplusplus
extern "C" {
#endif

#define SLAVE_MINIMUM_ADDRESS  0X93000000   /** Represents the minimum value of the address to be loaded from the core */
#define SLAVE_MAXIMUM_ADDRESS  0X93100000   /** Represents the maximum value of the address to be loaded from the core */
#define SLAVE_MINIMUN_CPUS      0  /** Minimum number of cpus on the board */
#define SLAVE_MAXIMUN_CPUS      4  /** Maximum number of cpus on the board */

/* initialize openamp module, including remoteproc, virtio, rpmsg */
int openamp_init(struct client_os_inst *client);

/* create endpoint for local */
int openamp_create_endpoint(const char *ep_name, rpmsg_ept_cb cb);

/* create endpoint for pty */
int openamp_create_pty_endpoint(const char *ep_name, rpmsg_ept_cb cb);

/* destroy endpoint for local */
int openamp_destroy_endpoint(const char *ep_name);

/* destroy endpoint for pty */
int openamp_destroy_pty_endpoint(struct pty_ep_data *pty_ep);

/* release openamp resource */
int openamp_deinit(struct client_os_inst *client);

/* get zero copy, the buffer_len reflects the maximum value of the buffer*/
void* get_zero_cpy_data_addr(const char *ep_name, int *buffer_len);

/* send message*/
int openamp_send_message(const char *ep_name, void *data, int data_len);

/* Determine whether the endpoint is ready */
int openamp_endpoint_ready(const char *ep_name);

#if defined __cplusplus
}
#endif

#endif  /* OPENAMP_MODULE_H */

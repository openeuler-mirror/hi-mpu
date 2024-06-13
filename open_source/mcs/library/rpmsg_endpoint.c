/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved
 *
 * SPDX-License-Identifier: MulanPSL-2.0
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>

#include "openamp_module.h"

#define RPMSG_SERVICE_NUM_ENDPOINTS 5

static struct {
	struct rpmsg_endpoint ep;
	const char *name;
	rpmsg_ept_cb cb;
	rpmsg_ns_unbind_cb unbind_cb;
	volatile bool bound;
} endpoints[RPMSG_SERVICE_NUM_ENDPOINTS];

static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
	rpmsg_destroy_ept(ept);
}

void ns_bind_cb(struct rpmsg_device *rdev,
					const char *name,
					uint32_t dest)
{
	int err;

	for (int i = 0; i < RPMSG_SERVICE_NUM_ENDPOINTS; ++i) {
		if (endpoints[i].name && (strcmp(name, endpoints[i].name) == 0)) {
			/* create the endpoint from host side and allocate an address */
			err = rpmsg_create_ept(&endpoints[i].ep,
						   rdev,
						   name,
						   RPMSG_ADDR_ANY,
						   dest,
						   endpoints[i].cb,
						   endpoints[i].unbind_cb);

			if (err != 0) {
				printf("Creating remote endpoint %s failed with error %d", name, err);
			} else {
				printf("found matched endpoint, creating %s with id:%d in host os\n",
						name, i);
				rpmsg_service_endpoint_bound(i);
				/* send an empty msg to notify the bound endpoint's address, so the other
				 * side can send data immediately, this may bring some potential issues like
				 * data buffer overrun, comment is out by default
				 */
				/* rpmsg_send(&endpoints[i].ep, (char *)"", 0); */
			}

			return;
		}
	}

	printf("Remote endpoint %s not registered locally\n", name);
}

int rpmsg_service_register_endpoint(const char *name, rpmsg_ept_cb cb,
						rpmsg_ns_unbind_cb unbind_cb, void *priv)
{
	if (name == NULL || cb == NULL) {
		return -1;
	}

	for (int i = 0; i < RPMSG_SERVICE_NUM_ENDPOINTS; ++i) {
		if (!endpoints[i].name) {
			endpoints[i].name = name;
			endpoints[i].cb = cb;
			endpoints[i].unbind_cb = unbind_cb ? unbind_cb : rpmsg_service_unbind;
			endpoints[i].ep.priv = priv;
			return i;
		}
	}

	printf("No free slots to register endpoint %s", name);

	return -1;
}

static int rpmsg_check_endpoint(struct rpmsg_endpoint *ept)
{
	if (ept == NULL || ept->rdev == NULL) {
		return -1;
	}

	rpmsg_destroy_ept(ept);
	return 0;
}

int rpmsg_service_unregister_endpoint(unsigned int endpoint_id)
{
	if (endpoint_id < RPMSG_SERVICE_NUM_ENDPOINTS) {
		if (endpoints[endpoint_id].name) {
			endpoints[endpoint_id].name = NULL;

			return rpmsg_check_endpoint(&endpoints[endpoint_id].ep);
		}
	}

	return -1;
}

bool rpmsg_service_endpoint_is_bound(unsigned int endpoint_id)
{
	if (endpoint_id < RPMSG_SERVICE_NUM_ENDPOINTS) {
		return endpoints[endpoint_id].bound;
	}

	return false;
}

void rpmsg_service_endpoint_bound(unsigned int endpoint_id)
{
	if (endpoint_id < RPMSG_SERVICE_NUM_ENDPOINTS) {
		endpoints[endpoint_id].bound = true;
	}
}

const char * rpmsg_service_endpoint_name(unsigned int endpoint_id)
{
	if (endpoint_id < RPMSG_SERVICE_NUM_ENDPOINTS) {
		return endpoints[endpoint_id].name;
	}

	return NULL;
}

int rpmsg_service_send(unsigned int endpoint_id, const void *data, size_t len)
{
	if (endpoint_id < RPMSG_SERVICE_NUM_ENDPOINTS) {
		return rpmsg_send(&endpoints[endpoint_id].ep, data, len);
	}

	return -1;
}

static int pthread_set_cpu_priority(int cpu_index, int priority)
{
    pthread_attr_t attr;
    struct sched_param s_param;
    pthread_attr_init(&attr);
    s_param.sched_priority = priority;
    pthread_t thread = pthread_self();
    pthread_setschedparam(thread, SCHED_FIFO, &s_param);

    int cpu_nums = sysconf(_SC_NPROCESSORS_CONF);
    if (cpu_index < 0 || cpu_index >= cpu_nums) {
        printf("cpu index ERROR!\n");
        return -1;
    }

    cpu_set_t cpumask;
    CPU_ZERO(&cpumask);
    CPU_SET(cpu_index, &cpumask);

    int ret = 0;
    ret = pthread_setaffinity_np(thread, sizeof(cpumask), &cpumask);
    if (ret < 0) {
        printf("set affinity np ERROR!\n");
        return -1;
    }

    struct sched_param s_param_master;
    s_param_master.sched_priority = priority;
    pid_t pid = getpid();
    sched_setscheduler(pid, SCHED_FIFO, &s_param_master);

    return 0;
}

void *rpmsg_service_receive_loop(void *arg)
{
    int priority = sched_get_priority_max(SCHED_FIFO);
    int cpu_affinity = 0;
    int ret;
    ret = pthread_set_cpu_priority(cpu_affinity, priority);
    if (ret < 0) {
        printf("set affinity and priority is ERROR!\n");
        return (void*)0;
    }

    struct client_os_inst *client = (struct client_os_inst *)arg;
    struct pollfd fds;
    fds.fd = client->mcs_fd;
    fds.events = POLLIN;

    int poll_timeout = -1;
	struct timespec start_time, current_time;
    while (1) {
        if (openamp_exiting) {
            printf("openamp is exiting.\n");
            break;
        }

        ret = poll(&fds, 1, poll_timeout);
        if (ret < 0) {
            printf("rpmsg_receive_message: poll failed.\n");
            break;
        }

        if (fds.revents & POLLIN) {
            virtqueue_notification(client->vq[0]);  /* will call endpoint_cb or ns_bind_cb */
            poll_timeout = 0; // Data may arrive, continue polling.
			clock_gettime(CLOCK_MONOTONIC, &start_time);
        } else {
			clock_gettime(CLOCK_MONOTONIC, &current_time);
			int elapsed_time = (current_time.tv_sec - start_time.tv_sec) * 1000000 +
						(current_time.tv_nsec - start_time.tv_nsec) / 1000;
            if (elapsed_time >= 20) { // 20us: Polling timeout and enter deep sleep.
                poll_timeout = -1;
            }
        }
    }

    return (void*)0;
}

struct rpmsg_endpoint *get_rpmsg_endpoint(const char *name)
{
	struct rpmsg_endpoint *ep = NULL;
	if (!name || strlen(name) == 0) {
		return ep;
	}
	
	for (int i = 0; i < RPMSG_SERVICE_NUM_ENDPOINTS; ++i) {
		if (endpoints[i].name && (strcmp(name, endpoints[i].name) == 0)) {
			return &endpoints[i].ep;
		}
	}

	return ep;
}

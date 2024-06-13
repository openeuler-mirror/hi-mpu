/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved
 *
 * SPDX-License-Identifier: MulanPSL-2.0
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include "openamp_module.h"

#define RPMSG_ENDPOINT_SYSTEM  "shutdown"
#define HI_CORE3_SIZE  0x100000

bool openamp_exiting;
struct openamp_data *g_openamp_data;
static void *virt_shared_mem = NULL;
static unsigned int virt_shared_mem_size;

static int reserved_mem_init(struct client_os_inst *client)
{
    int bin_fd;
    struct stat buf;
    void *file_addr, *sh_bin_addr;
    int bin_size;
    void *sh_mem_addr;
    unsigned long state_arg = client->cpu_id;

    /* to-do: check if the starting address of client os is valid */
    /* to-do: client os must be in the range of shared mem */
    /* shared memory for virtio */
    sh_mem_addr = mmap(NULL, client->shared_mem_size,
            PROT_READ | PROT_WRITE, MAP_SHARED, client->mcs_fd,
            client->phy_shared_mem);
    if (sh_mem_addr == MAP_FAILED) {
        printf("mmap failed: sh_mem_addr:%p\n", sh_mem_addr);
        return -EPERM;
    }
    virt_shared_mem = sh_mem_addr;
    virt_shared_mem_size = client->shared_mem_size;
    /* must be initialized to zero */
    memset(sh_mem_addr, 0, client->shared_mem_size);

    /* open clientos bin file from Linux file system */
    bin_fd = open(client->path, O_RDONLY);
    if (bin_fd < 0) {
        printf("open %s failed, bin_fd:%d\n", client->path, bin_fd);
        goto err_ummap_share_mem;
    }
    /* memory for loading clientos bin file */
    fstat(bin_fd, &buf);
    bin_size = PAGE_ALIGN(buf.st_size);

    /* the address in the shared memory to put bin file */
    sh_bin_addr = mmap(NULL, HI_CORE3_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                    client->mcs_fd, client->load_address);
    if (sh_bin_addr == MAP_FAILED) {
        printf("mmap reserved mem failed: sh_bin_addr:%p\n",
                sh_bin_addr);
        goto err_close_bin_fd;
    }
    
    /* the address of bin file in Linux */
    file_addr = mmap(NULL, HI_CORE3_SIZE, PROT_READ, MAP_PRIVATE, bin_fd, 0);
    if (file_addr == MAP_FAILED) {
        printf("mmap failed: file_addr: %p\n", file_addr);
        goto err_ummap_sh_bin_addr;
    }

    /* load clientos */
    if (bin_size < buf.st_size) {
        printf("The file storage size is exceededr\n");
        munmap(file_addr, bin_size);
        goto err_ummap_sh_bin_addr;
    }
    memcpy(sh_bin_addr, file_addr, buf.st_size);
    close(bin_fd);

    /* unmap bin file, both from the Linux and shared memory */
    munmap(file_addr, bin_size);
    munmap(sh_bin_addr, bin_size);

    client->virt_shared_mem = sh_mem_addr;
    client->vdev_status_reg = sh_mem_addr;
    client->virt_tx_addr = sh_mem_addr + client->shared_mem_size - client->vdev_status_size;
    client->virt_rx_addr = client->virt_tx_addr - client->vdev_status_size;

    return 0;

err_ummap_sh_bin_addr:
    munmap(sh_bin_addr, bin_size);
err_close_bin_fd:
    close(bin_fd);
err_ummap_share_mem:
    munmap(sh_mem_addr, client->shared_mem_size);
    ioctl(client->mcs_fd, IOC_CLOSE_OPENAMP, &state_arg);
    return -1;
}

static void reserved_mem_release(struct client_os_inst *client)
{
    if (client->virt_shared_mem) {
        munmap(client->virt_shared_mem, client->shared_mem_size);
    }

    if (client->mcs_fd >= 0) {
        unsigned long state_arg = 0;
        ioctl(client->mcs_fd, IOC_CLOSE_OPENAMP, &state_arg);
        close(client->mcs_fd);
    }
}

static void pty_endpoint_unbind_cb(struct rpmsg_endpoint *ept)
{
    if (ept == NULL) {
        printf("get unbind request from client side  is NULL\n");
        return;
    }

    printf("%s: get unbind request from client side\n", ept->name);

    struct pty_ep_data *pty_ep = (struct pty_ep_data *)ept->priv;

    openamp_destroy_pty_endpoint(pty_ep);
}

static int endpoint_cb_shutdown(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
    return 0;
}

static void *openamp_thread(void *arg)
{
    struct client_os_inst *client = (struct client_os_inst *)arg;
    rpmsg_service_receive_loop(client);
}

/** create salve pthread for openamp */
static int create_openamp_thread(struct client_os_inst *client)
{
    struct openamp_data *tmp_openamp_data;
    tmp_openamp_data = (struct openamp_data *)malloc(sizeof(struct openamp_data));
    if (tmp_openamp_data == NULL) {
        printf("tmp_openamp_data NULL\n");
        return -1;
    }

    g_openamp_data = tmp_openamp_data;
    if (pthread_create(&g_openamp_data->openamp_thread, NULL, openamp_thread, client) < 0) {
        printf("openamp thread create failed\n");
        free(tmp_openamp_data);
        g_openamp_data = NULL;
        return -1;
    }

    return 0;
}

static int check_openamp_client_params(struct client_os_inst *client)
{
    if (client == NULL) {
        printf("Initialization failure for client name!\n");
        return -1;
    }

    if (client->load_address < SLAVE_MINIMUM_ADDRESS || client->load_address > SLAVE_MAXIMUM_ADDRESS) {
        printf("The load start address from the core exceeds the range value\n");
        return -1;
    }

    if (client->cpu_id < SLAVE_MINIMUN_CPUS  || client->cpu_id > (SLAVE_MAXIMUN_CPUS - 1)) {
        printf("The number of cpus exceeds the limit. Procedure\n");
        return -1;
    }

    int ret;
    ret = open(MCS_DEVICE_NAME, O_RDWR | O_SYNC);
    if (ret < 0) {
        printf("open %s device failed\n", MCS_DEVICE_NAME);
        return ret;
    }

    unsigned long state_arg;
    state_arg = client->cpu_id;
    client->mcs_fd = ret;
    ret = ioctl(client->mcs_fd, IOC_AFFINITY_INFO, &state_arg);
    if (ret < 0) {
        printf("acquire cpu state failed\n");
        close(client->mcs_fd);
        return -1;
    }

    if (state_arg != CPU_STATE_OFF) {
        printf("cpu(%d) is already on.\n", client->cpu_id);
        close(client->mcs_fd);
        return -1;
    }

    return 0;
}

static void rpmsg_release_sources(struct client_os_inst *client)
{
    destory_remoteproc(client);
    virtio_deinit(client);
    reserved_mem_release(client);
}

int openamp_init(struct client_os_inst *client)
{
    int ret;

    ret = check_openamp_client_params(client);
    if (ret < 0) {
        return -1;
    }

    client->entry = client->load_address;
    ret = create_remoteproc(client);
    if (ret < 0) {
        printf("create remoteproc failed\n");
        close(client->mcs_fd);
        return ret;
    }

    ret = reserved_mem_init(client);
    if (ret < 0) {
        printf("failed to init reserved mem\n");
        destory_remoteproc(client);
        close(client->mcs_fd);
        return ret;
    }

    ret = virtio_init(client);
    if (ret < 0) {
        printf("failed to init reserved mem\n");
        destory_remoteproc(client);
        reserved_mem_release(client);
        return ret;
    }

    ret = rpmsg_sys_service_init();
    if (ret < 0) {
        printf("failed to init reserved mem\n");
        rpmsg_release_sources(client);
        return -1;
    }

    ret = remoteproc_start(&client->rproc);
    if (ret < 0) {
        printf("start processor failed\n");
        rpmsg_release_sources(client);
        return -1;
    }

    ret = create_openamp_thread(client);
    if (ret < 0) {
        printf("create openamp thread failed\n");
        rpmsg_release_sources(client);
        return -1;
    }

    int register_endpoint_cpuoff = openamp_create_endpoint(RPMSG_ENDPOINT_SYSTEM, endpoint_cb_shutdown);
    if (register_endpoint_cpuoff < 0) {
        /** release resource */
        rpmsg_release_sources(client);
        return -1;
    }

    return 0;
}

static void openamp_release_resource(struct client_os_inst *client)
{
    destory_remoteproc(client); /* shutdown clientos first */
    virtio_deinit(client);
    reserved_mem_release(client);
    if (!g_openamp_data) {
        printf("Failed to destory the g_openamp_data\n");
    } else {
        free(g_openamp_data);
        g_openamp_data = NULL;
    }
}

int openamp_deinit(struct client_os_inst *client)
{
    printf("\nOpenAMP ended.\n");
    if (client == NULL) {
        printf("Destruction failure for client name!\n");
        return -1;
    }
    openamp_exiting = true;

    struct rpmsg_endpoint *ept_power_off = get_rpmsg_endpoint(RPMSG_ENDPOINT_SYSTEM);
    if (ept_power_off == NULL) {
        printf("The power-off channel does not exist\n");
        openamp_release_resource(client);
        return -1;
    }

    char *notify_power_off = "stop";
    int buffer_len = 0;
    char *buffer_addr = get_zero_cpy_data_addr(RPMSG_ENDPOINT_SYSTEM, &buffer_len);
    if (buffer_addr == NULL) {
        printf("No buffer is available for %s\n", RPMSG_ENDPOINT_SYSTEM);
        openamp_release_resource(client);
        return openamp_destroy_endpoint(RPMSG_ENDPOINT_SYSTEM);
    }

    if (strlen(notify_power_off) > (buffer_len - 1)) {
        printf("No buffer is available for %s\n", RPMSG_ENDPOINT_SYSTEM);
        return -1;
    }

    strcpy(buffer_addr, notify_power_off);
    int ret;
    ret = openamp_send_message(RPMSG_ENDPOINT_SYSTEM, (void *)buffer_addr,
                               strlen(notify_power_off) * sizeof(char));
    if (ret < 0) {
        printf("Failed to notify the power-off command\n");
        ept_power_off->name[0] = NULL;
        return -1;
    }

    openamp_release_resource(client);
    ept_power_off->name[0] = NULL;

    return openamp_destroy_endpoint(RPMSG_ENDPOINT_SYSTEM);
}

int openamp_create_endpoint(const char *ep_name, rpmsg_ept_cb cb)
{
    if (ep_name == NULL || cb == NULL) {
        return -1;
    }

    int ret;
    ret = rpmsg_service_register_endpoint(ep_name, cb, rpmsg_destroy_ept, NULL);
    if (ret < 0) {
        return -1;
    }

    return 0;
}

int openamp_create_pty_endpoint(const char *ep_name, rpmsg_ept_cb cb)
{
    if (ep_name == NULL || cb == NULL) {
        return -1;
    }

    struct pty_ep_data *pty_ep;
    pty_ep = (struct pty_ep_data *)malloc(sizeof(struct pty_ep_data));
    if (pty_ep == NULL) {
        return -1;
    }

    int ret;
    ret = open_pty(&pty_ep->fd_master);
    if (ret != 0) {
        free(pty_ep);
        return -1;
    }

    pty_ep->ep_id = rpmsg_service_register_endpoint(ep_name, cb, pty_endpoint_unbind_cb, pty_ep);
    if (pthread_create(&pty_ep->pty_thread, NULL, pty_thread, pty_ep) < 0) {
        printf("pty thread create failed\n");
        free(pty_ep);
        return -1;
    }

    pthread_detach(pty_ep->pty_thread);

    return 0;
}

/* destroy endpoint for local */
int openamp_destroy_endpoint(const char *ep_name)
{
    struct rpmsg_endpoint *ept = get_rpmsg_endpoint(ep_name);
    if (ept == NULL) {
        printf("rpmsg_endpoint is not exist\n");
        return -1;
    }

    rpmsg_destroy_ept(ept);
    return 0;
}

int openamp_destroy_pty_endpoint(struct pty_ep_data *pty_ep)
{
    if (pty_ep == NULL) {
        printf("enter pty_endpoint_exit name = NULL");
        return -1;
    }

    /* release the resources */
    int ret = rpmsg_service_unregister_endpoint(pty_ep->ep_id);
    if (ret < 0) {
        printf("enter pty_endpoint_exit name = NULL");
        return -1;
    }

    close(pty_ep->fd_master);
    pthread_cancel(pty_ep->pty_thread);
    free(pty_ep);
    return 0;
}

void* get_zero_cpy_data_addr(const char *ep_name, int *buffer_len)
{
    struct rpmsg_endpoint *ept = get_rpmsg_endpoint(ep_name);
    if (ept == NULL) {
        printf("rpmsg_endpoint is not exist\n");
        return NULL;
    }

    int wait = 1;
    return rpmsg_get_tx_payload_buffer(ept, buffer_len, wait);
}

static int check_send_message_address(void *data, int data_len)
{
    unsigned long long tmp = (unsigned long long)(uintptr_t)data + (unsigned long long)data_len;
    unsigned long long max_virt_shared_address = (unsigned long long)(uintptr_t)virt_shared_mem + 
                                    (unsigned long long)virt_shared_mem_size;
    if ((data < virt_shared_mem) || (((unsigned long long)(uintptr_t)data) > max_virt_shared_address) ||
        (tmp < ((unsigned long long)(uintptr_t)virt_shared_mem)) || (tmp > max_virt_shared_address)) {
        return -1;
    }

    return 0;
}

int openamp_send_message(const char *ep_name, void *data, int data_len)
{
    int ret;
    ret = check_send_message_address(data, data_len);
    if (ret < 0) {
        printf("Illegal address\n");
        return -1;
    }

    int header_len = 16;
    if (data_len > (RPMSG_BUFFER_SIZE - header_len)) {
        printf("The data length should not exceed : %d byte\n", (RPMSG_BUFFER_SIZE - header_len));
        return -1;
    }

    struct rpmsg_endpoint *ept = get_rpmsg_endpoint(ep_name);
    if (ept == NULL) {
        printf("rpmsg_endpoint is not exist\n");
        return -1;
    }

    return rpmsg_send_offchannel_nocopy_hwi(ept, data, data_len);
}

int openamp_endpoint_ready(const char *ep_name)
{
    struct rpmsg_endpoint *ept = get_rpmsg_endpoint(ep_name);

    if (ept == NULL) {
        printf("rpmsg_endpoint is not exist\n");
        return -1;
    }

    return is_rpmsg_ept_ready(ept);
}

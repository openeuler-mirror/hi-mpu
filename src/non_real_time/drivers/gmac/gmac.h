// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Header file of the gmac driver.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _GMAC_H_
#define _GMAC_H_

#include <linux/list.h>
#include <linux/version.h>
#include "com_phy.h"

extern u32 g_rx_mode;

typedef void (*VOIDFUNCPTR)(const struct net_device *, const struct sk_buff *);

#define GMAC_OK    0

#define ETH0 0
#define ETH1 1
#define ETH2 2
#define ETH3 3

#define MASK_PHY_CTRL_SPEED_6  0x0040
#define MASK_PHY_CTRL_SPEED_13 0x2000
#define MASK_PHY_CTRL_DUPLEX   0x0100

#define CHIP_RX_FIFO_NUM (127)
#define CHIP_TX_FIFO_NUM (64)

#define GMAC_RX_BD_RING_SIZE (CHIP_RX_FIFO_NUM)
#define GMAC_TX_BD_RING_SIZE (CHIP_TX_FIFO_NUM)

#define MAX_MAC_PORT_NUM 4

#define GMAC_DEV_NAME "hi1711_gmac_net"

#ifndef REG32
#define REG32(Addr) (*(volatile u32 *const)(Addr))
#endif

#define gmac_reg_write(addr, value) \
    (REG32(addr) = (value))

#define gmac_reg_read(addr, value) \
    ((value) = REG32(addr))

#define SIOCDEVDRVCMD 0x89F4

#define ETH_DRV_LOOP_SET          0x00000001
#define ETH_DRV_PHY_REINIT        0x00000002
#define ETH_DRV_RMII_CLK_CONFIG   0x00000003
#define ETH_SERDES_POWER_SET      0x00000004
#define ETH_SERDES_NEG_STATE      0x00000005
#define ETH_DRV_AUTONEGO_SET      0x00000009
#define ETH_DRV_SPPED_DUPLEX_SET  0x0000000a

#define ETH_TEST_DRV_LOOP      0x00000011
#define ETH_TEST_DRV_SHORT_PKT 0x00000012

#define CPU_CACHE_LINE_SIZE  64
#define PCU_CACHE_LINE_SIZE  32

#define GMAC_REGDUMP_LEN  0x600

#define RX_TIMER_BLANK    20 /* ms */

/* The unit is cache line size. */
#define GMAC_RX_SKIP1  0x00
#define GMAC_RX_SKIP2  0x01
#define GMAC_RX_DESCR  0x01

#define DEFAULT_MTU_SIZE  1500
#define MAX_MTU_SIZE      7168
/*
 * Because there may be a 0 to 3 byte alignment field at the beginning of the frame content,
 * ensure that the maximum frame length of the configuration hardware is not greater than MAX_FRAME_SIZE - 4
 */
#define MAX_FRAME_SIZE (MAX_MTU_SIZE + PCU_CACHE_LINE_SIZE)

#define PACKET_HEAD_SIZE ((GMAC_RX_SKIP1 + GMAC_RX_SKIP2 + GMAC_RX_DESCR) * PCU_CACHE_LINE_SIZE)
#define PACKET_DATA_SIZE MAX_FRAME_SIZE
#define RX_BUFFER_SIZE   (PACKET_HEAD_SIZE + PACKET_DATA_SIZE)

#define GMAC_MAX_PKT_LEN MAX_FRAME_SIZE /* keep same with PACKET_DATA_SIZE */

/* The default MTU is 1500.GMAC Set the maximum TX/RX frame length to 1500+14+4+4*2+2 = 1528 (4-byte alignment) */
#define DEFUALT_MAX_FRAEM_LEN 1528

/* Used for packet descriptors. The descriptors provided by the hardware are big-endian.. */
#define GMAC_DESC_LEN (6 * 4)

#define GMAC_DESC_PKT_LEN_HIGH 8
#define GMAC_DESC_PKT_LEN_LOW  9

#define GMAC_DESC_PORT_NUM      10
#define GMAC_DESC_PORT_NUM_MASK 0xf0

#define GMAC_DESC_IP_TCP_UDP        16
#define GMAC_DESC_IP_TCP_UDP_SHIFT  6
#define GMAC_DESC_IP_TCP_UDP_MASK   0xC0

#define GMAC_DESC_IPSEC         16
#define GMAC_DESC_IPSEC_SHIFT   5
#define GMAC_DESC_IPSEC_MASK    0x20

#define GMAC_DESC_IPVX          16
#define GMAC_DESC_IPVX_SHIFT    4
#define GMAC_DESC_IPVX_MASK     0x10

#define GMAC_DESC_L4_ERR_HIGH      16
#define GMAC_DESC_L4_ERR_HIGH_MASK 0x7

#define GMAC_DESC_L4_ERR_LOW      17
#define GMAC_DESC_L4_ERR_SHIFT    7
#define GMAC_DESC_L4_ERR_LOW_MASK 0x80

#define GMAC_DESC_FRAG_FLAG      17
#define GMAC_DESC_FRAG_FLAG_MASK 0x40

#define GMAC_DESC_OPT_FLAG      17
#define GMAC_DESC_OPT_SHIFT     5
#define GMAC_DESC_OPT_FLAG_MASK 0x20

#define GMAC_DESC_IP_VER_ERR        17
#define GMAC_DESC_IP_VER_ERR_SHIFT  4
#define GMAC_DESC_IP_VER_ERR_MASK   0x10

#define GMAC_DESC_BRD_CST           17
#define GMAC_DESC_BRD_CST_SHIFT     3
#define GMAC_DESC_BRD_CST_MASK      0x08

#define GMAC_DESC_MUL_CST           17
#define GMAC_DESC_MUL_CST_SHIFT     2
#define GMAC_DESC_MUL_CST_MASK      0x04

#define GMAC_DESC_ARP      17
#define GMAC_DESC_ARP_MASK 0x02

#define GMAC_DESC_RARP      17
#define GMAC_DESC_RARP_MASK 0x01

#define GMAC_DESC_ICMP          18
#define GMAC_DESC_ICMP_SHIFT    7
#define GMAC_DESC_ICMP_MASK     0x80

#define GMAC_DESC_VLAN_FLAG         18
#define GMAC_DESC_VLAN_FLAG_SHIFT   6
#define GMAC_DESC_VLAN_FLAG_MASK    0x40

#define GMAC_DESC_DROP      18
#define GMAC_DESC_DROP_MASK 0x20

#define GMAC_DESC_L3_ERR      18
#define GMAC_DESC_L3_ERR_MASK 0x1E

#define GMAC_DESC_L2_ERR      18
#define GMAC_DESC_L2_ERR_MASK 0x01

#define GMAC_DESC_KEY_MATCH      19
#define GMAC_DESC_KEY_SHFIT      7
#define GMAC_DESC_KEY_MATCH_MASK 0x80

#define TX_TIMEOUT (2 * HZ) /* Transmitter timeout */

#define GMAC_TX_IDLE  0x77777777
#define GMAC_TX_START 0x5a5a5a5a
#define GMAC_TX_END   0x00000000

#define ENABLE  (0x1)
#define DISABLE (0x0)

#define GMAC_DELAYED_WORK_TIMEOUT 1000 /* ms */

/* Macro Definitions Related to Loopback Tests */
#define GMAC_LOOP_DATA_SIZE 0x100
#define GMAC_LOOP_NUM       0x03
#define GMAC_LOOP_WAIT      (GMAC_LOOP_NUM * 50)

#define DRV_MODULE_NAME    "1711_eth"
#define DRV_MODULE_VERSION "2.00"

#define ETH_TOOL_VER_LEN 32

#define GMAC_NUM_STATS (sizeof(struct gmac_ethtool_stats) / sizeof(u64))

#define ETH_P_8021Q_V 0x0081
#define ETH_P_IPV6_V  0xdd86

#define NEXTHDR_ICMP                 58
#define MAC_ADDR_SIZE                6
#define NDISC_NEIGHBOUR_SOLICITATION 135
#define ETH_HEAD_OFFSET              34

#define GMAC_NUM_BIT_MASK      4
#define GMAC_DMA_MASK          (~0ULL)

#define GMAC_PKT_LEN_H_OFFSET 8

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#define NEW_MTU_CHECK (new_mtu < 256)
#else
#define NEW_MTU_CHECK ((new_mtu < 256) || (new_mtu > MAX_MTU_SIZE))
#endif

struct vlan_hdr__ {
    u8 dest[MAC_ADDR_SIZE];
    u8 source[MAC_ADDR_SIZE];
    u16 vlan_proto;
    u16 vlan_tci;
    u16 eth_proto;
};

typedef struct gmac_devs {
    struct net_device *mac_devs[MAX_MAC_PORT_NUM];
} gmac_devs_s;

typedef struct gmac_phy_info {
    char *name;
    u32 phy_id;
    phy_ops_s *driver;
} gmac_phy_info_s;

typedef enum loop_mode {
    ETH_NO_LOOP = 0,
    ETH_MAC_SYS_LOOP,
    ETH_MAC_LINE_LOOP,
    ETH_SERDES_SYS_LOOP,
    ETH_SERDES_LINE_LOOP,
    ETH_UNKNOWN_LOOP_MODE
} loop_mode_e;

typedef enum gmac_direction {
    GMAC_TX,
    GMAC_RX,
    GMAC_RX_DESCRIPTION
} gmac_direction_e;

typedef enum gmac_l3_err_code {
    GMAC_L3_OK,
    GMAC_L3_WRONG_HEAD,
    GMAC_L3_CSUM_ERR,
    GMAC_L3_LEN_ERR,
    GMAC_L3_ZERO_TTL,
    GMAC_L3_RSVD,
} gmac_l3_err_code_e;

typedef enum gmac_l4_err_code {
    GMAC_L4_OK,
    GMAC_L4_WRONG_HEAD,
    GMAC_L4_LEN_ERR,
    GMAC_L4_CSUM_ERR,
    GMAC_L4_ZERO_PORT_NUM,
    GMAC_L4_RSVD,
} gmac_l4_err_code_e;

typedef enum gmac_pkt_type_code {
    GMAC_NO_IP_PKT,
    GMAC_IP_PKT,
    GMAC_TCP_PKT,
    GMAC_UDP_PKT,
} gmac_pkt_type_e;

typedef struct {
    u32 tx_state;
    dma_addr_t tx_state_dma_addr;

    struct sk_buff *tx_skb_buff;
    dma_addr_t skb_data_dma_addr;
} gmac_tx_buffer_s;

struct gmac_ethtool_stats {
    u64 rx_desc_l3_wrong_head_cnt;
    u64 rx_desc_l3_csum_err_cnt;
    u64 rx_desc_l3_len_err_cnt;
    u64 rx_desc_l3_zero_ttl_cnt;
    u64 rx_desc_l3_rsvd_cnt;
    u64 rx_desc_l4_wrong_head_cnt;
    u64 rx_desc_l4_len_err_cnt;
    u64 rx_desc_l4_csum_err_cnt;
    u64 rx_desc_l4_zero_port_num_cnt;
    u64 rx_desc_l4_rsvd_cnt;
    u64 rx_desc_frag_cnt;
    u64 rx_desc_ip_ver_err_cnt;
    u64 rx_desc_ipv4_pkt_cnt;
    u64 rx_desc_ipv6_pkt_cnt;
    u64 rx_desc_no_ip_pkt_cnt;
    u64 rx_desc_ip_pkt_cnt;
    u64 rx_desc_tcp_pkt_cnt;
    u64 rx_desc_udp_pkt_cnt;
    u64 rx_desc_vlan_pkt_cnt;
    u64 rx_desc_icmp_pkt_cnt;
    u64 rx_desc_arp_pkt_cnt;
    u64 rx_desc_rarp_pkt_cnt;
    u64 rx_desc_multicast_pkt_cnt;
    u64 rx_desc_broadcast_pkt_cnt;
    u64 rx_desc_ipsec_pkt_cnt;
    u64 rx_desc_ip_opt_pkt_cnt;
    u64 rx_desc_key_match_cnt;
    /* Rx statistics INFO */
    u64 rx_octets_total_ok_cnt;
    u64 rx_uc_pkts_cnt;
    u64 rx_mc_pkts_cnt;
    u64 rx_bc_pkts_cnt;
    u64 rx_vlan_pkt_cnt;
    u64 rx_fw_ctrl_frame_cnt;
    u64 rx_octets_bad_cnt;
    u64 rx_octets_total_filt_cnt;
    u64 rx_filt_pkt_cnt;

    u64 rx_framsize_64;
    u64 rx_framsize_65_127;
    u64 rx_framsize_128_255;
    u64 rx_framsize_256_511;
    u64 rx_framsize_512_1023;
    u64 rx_framsize_1024_1518;
    u64 rx_framsize_bt_1518;

    u64 rx_fcs_error_cnt;
    u64 rx_data_error_cnt;
    u64 rx_align_error_cnt;
    u64 rx_frame_long_err_cnt;
    u64 rx_frame_very_long_err_cnt;
    u64 rx_frame_runt_err_cnt;
    u64 rx_frame_short_err_cnt;
    u64 rx_overrun_cnt;
    u64 rx_rbuff_rq_err_cnt;

    u64 tx_octets_total_ok_cnt;
    u64 tx_uc_pkts_cnt;
    u64 tx_mc_pkts_cnt;
    u64 tx_bc_pkts_cnt;
    u64 tx_vlan_pkt_cnt;
    u64 tx_fw_ctrl_frame_cnt;
    u64 tx_octets_bad_cnt;

    u64 tx_framsize_64;
    u64 tx_framsize_65_127;
    u64 tx_framsize_128_255;
    u64 tx_framsize_256_511;
    u64 tx_framsize_512_1023;
    u64 tx_framsize_1024_1518;
    u64 tx_framsize_bt_1518;

    u64 tx_fcs_error_cnt;
    u64 tx_underrun_err_cnt;
    u64 tx_add_cs_fail_cnt;
    u64 tx_buffrl_err_cnt;
    u64 tx_we_err_cnt;

    u64 rx_overrun_drop_cnt;

    u64 tx_excessive_drop_cnt;
    u64 tx_overrun_drop_cnt;
};

typedef struct {
    gmac_tx_buffer_s tx_buf; /* This field must be the first field in the structure. */
    struct list_head entry;
} gmac_queue_node_t;

typedef struct {
    spinlock_t lock;
    u32 list_count;
    struct list_head head;
} gmac_queue_t;

/* struct gmac_private */
typedef struct gmac_hi1711_private_t {
    u32 id;
    struct napi_struct napi;
    struct sk_buff *rx_bd_ring[GMAC_RX_BD_RING_SIZE];
    u32 rx_skb_curr;
    u32 rx_skb_remain; /* Number of packets that are not processed by the software after the last NAPI scheduling */
    gmac_tx_buffer_s *tx_bd_ring[GMAC_TX_BD_RING_SIZE]; /* tx buffer and write-back address */
    u32 virq;
    spinlock_t lock;
    struct net_device_stats status;

    struct phy_device *phy;

    s32 oldduplex, oldspeed, oldlink;
    s32 oldautoneg;

    struct gmac_ethtool_stats ges;

    volatile u32 quota;
    spinlock_t mac_lock;
    s32 opened;
    u32 drop_count;

    VOIDFUNCPTR gmac_loop_recv_hook;
    u32 gmac_loop_cnt;
    /* Compares the current jiffies to determine whether to read the TX/RX data of hardware statistics INFO. */
    u64 update_jiffies;

    phy_ops_s *phy_handle;
    u32 phy_addr;

    u8 *tx_bd_start_vaddr;
    dma_addr_t tx_bd_start_dma_addr;
    gmac_queue_t gmac_txbd_unused;
    gmac_queue_t gmac_txbd_inused;
    struct tasklet_struct tx_free_tasklet;
} gmac_private_s;

typedef struct gmac_pkt_store_ctrl_cfg_t {
    u32 cf_buf_1st_skip1;
    u32 cf_buf_n1st_skip;
    u32 cf_rx_align_num;
    u32 cf_cache_line;
    u32 cf_rxbuf_1st_skip_size2;
} gmac_pkt_store_ctrl_cfg_s;

typedef struct gmac_rx_pkt_mode_cfg_t {
    u32 cf_instr_head_mode;
    u32 cf_skip_len;
    u32 cf_parse_mode;
    u32 mac_skip_len;
} gmac_rx_pkt_mode_cfg_s;

typedef struct fe_pcu_cmd_t {
    union {
        struct {
            u32 l4_cs : 1;
            u32 wb : 1;    /* When Wb is 1, write 0 to the WORD3 address after the packet transmission is complete. */
            u32 l3_cs : 1; /* Checksum is required. */
            u32 reserved2 : 16;
            u32 ds : 6; /* Number of data segments. This parameter is valid only when Sd is 1. */
           /*
            * When 0, the frame contains only one segment, pointing to the segment;
            * When 1, the frame contains multiple segments
            */
            u32 sd : 1;
            u32 ipoff : 5; /* Indicates the start offset of the IP packet header in the datagram. */
            u32 reserved1 : 1;
        } bits;
        u32 u_32;
    } word0;

    union {
        struct {
            u32 pool : 4;
            u32 len : 16;
            u32 back : 6;
            u32 fm : 1;
            u32 l2cache : 1;
            u32 reserve : 4;
        } bits;
        u32 u_32;
    } word1;

    u32 pkt_addr;
    u32 clear_addr;
} fe_pcu_cmd_s;

typedef struct {
    u32 loop_cmd;
    loop_mode_e loop_mode;
} eth_drv_loop_set_s;

typedef struct {
    /*
     * Indicates whether to enable GMAC autonegotiation. Generally, GMAC autonegotiation
     * needs to be disabled when the switch is connected. The default value is enabled.
     */
    u32 cmd;
    u32 gmac_auto_nego_enable; /* Whether to enable PHY auto-negotiation */
    u32 phy_auto_nego_enable;
} eth_drv_auto_nego_set_s;

typedef struct {
    u32 cmd;
    u32 speed;
    u32 duplex;
} eth_drv_speed_duplex_set_s;

typedef struct eth_drv_line_loop_t {
    u32 cmd;
    u32 loop_en;
    loop_mode_e loop_mode;
} eth_drv_line_loop_s;

typedef struct eth_drv_phy_mode_set_t {
    u32 cmd;
    u32 auto_neg;
    u32 speed;
    u32 duplex;
} eth_drv_phy_mode_set_s;

typedef struct eth_drv_phy_link_status_t {
    u32 cmd;
    u32 link_status;
} eth_drv_phy_link_status_s;

typedef struct eth_test_loop_t {
    u32 cmd;
    u32 enable;
} eth_test_loop_s;

typedef struct eth_serdes_power_ctr_t {
    u32 cmd;
    u32 serdes_power_mode;
} eth_serdes_power_ctr_s;

struct eth_drv_phy_info {
    u32 cmd;
    u32 addr;
};

struct eth_drv_rmii_config {
    u32 cmd;
    u32 clk_sel; /* 0: external clock1: internal clock */
    u32 clk_edge_sel; /* 0: TX on the rising edge; 1: TX on the falling edge */
    u32 dri_power;
    u32 reserved;
};

struct eth_serdes_neg_state {
    u32 cmd;
    u32 state; /* SerDes auto-negotiation status */
};

/* Basic Information About Interrupt Handling */
typedef struct {
    u32 intr_mask;
    u32 intr_rsv1;
    irqreturn_t intr_ret;
    void (*intr_proc)(struct net_device *dev);
} intr_disp_info_s;

extern const gmac_devs_s *g_gmac_dev;

static inline void gmac_queue_init(gmac_queue_t *q)
{
    q->list_count = 0;
    spin_lock_init(&q->lock);
    INIT_LIST_HEAD(&q->head);
}

static inline s32 gmac_queue_is_empty(gmac_queue_t *q)
{
    return list_empty(&q->head);
}

static inline gmac_queue_node_t *gmac_queue_head(gmac_queue_t *q)
{
    if (gmac_queue_is_empty(q)) {
        return NULL;
    }

    return list_first_entry(&q->head, gmac_queue_node_t, entry);
}

static inline void gmac_queue_handle_enque(gmac_queue_t *q, gmac_queue_node_t *node)
{
    q->list_count++;
    list_add_tail(&node->entry, &q->head);
}

static inline gmac_queue_node_t *gmac_queue_handle_deque(gmac_queue_t *q)
{
    gmac_queue_node_t *n = NULL;

    if (gmac_queue_is_empty(q)) {
        return NULL;
    }

    q->list_count--;
    n = list_first_entry(&q->head, gmac_queue_node_t, entry);
    list_del(&n->entry);
    return n;
}

static inline s32 gmac_queue_check_empty(gmac_queue_t *q)
{
    s32 ret;

    spin_lock_bh(&q->lock);
    ret = gmac_queue_is_empty(q);
    spin_unlock_bh(&q->lock);

    return ret;
}

static inline u32 gmac_queue_get_count(gmac_queue_t *q)
{
    u32 count;

    spin_lock_bh(&q->lock);
    count = q->list_count;
    spin_unlock_bh(&q->lock);

    return count;
}

static inline s32 gmac_queue_enque(gmac_queue_t *q, gmac_queue_node_t *node,
                                   s32(*cb)(gmac_queue_node_t *, void *, void *), void *arg1, void *arg2)
{
    s32 ret = 0;

    spin_lock_bh(&q->lock);
    ret = cb ? cb(node, arg1, arg2) : ret;
    if (!ret) {
        gmac_queue_handle_enque(q, node);
    }
    spin_unlock_bh(&q->lock);

    return ret;
}

static inline gmac_queue_node_t *gmac_queue_deque(gmac_queue_t *q)
{
    gmac_queue_node_t *n = NULL;

    spin_lock_bh(&q->lock);
    n = gmac_queue_handle_deque(q);
    spin_unlock_bh(&q->lock);

    return n;
}

/* Internal function prototype description */
s32 gmac_open(struct net_device *net_dev);
s32 gmac_stop(struct net_device *net_dev);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
void gmac_tx_timeout(struct net_device *net_dev, u32 txqueue);
#else
void gmac_tx_timeout(struct net_device *net_dev);
#endif
s32 gmac_set_mac_addr(struct net_device *net_dev, void *pstPtr);
s32 gmac_ioctl(struct net_device *net_dev, struct ifreq *pstIfr, s32 lCmd);
s32 gmac_ioctl_from_usr_space(struct net_device *net_dev, const struct ifreq *ifr, void __user *data, u32 eth_cmd);
s32 gmac_hw_set_port_mode(const struct net_device *dev, u32 speed, u32 duplex);
u32 gmac_hw_set_sgmii_an_enable(const struct net_device *dev);
void gmac_interrupt_handle_rx_intrpt(struct net_device *dev);
void gmac_interrupt_handle_buf_avl(struct net_device *dev);
void gmac_interrupt_handle_mac_fifo_err(struct net_device *dev);
void gmac_interrupt_handle_tx_bd(struct net_device *dev);
void gmac_interrupt_handle_tx_ahb_err(struct net_device *dev);
void gmac_interrupt_handle_tx_release_err(struct net_device *dev);
void gmac_interrupt_handle_tx_drop_err(struct net_device *dev);
void gmac_interrupt_handle_rx_drop_err(struct net_device *dev);
void gmac_interrupt_handle_rx_ahb_err(struct net_device *dev);
void gmac_interrupt_handle_rbreq_err(struct net_device *dev);
void gmac_interrupt_handle_we_err(struct net_device *dev);
s32 gmac_cr_write(u32 id, u32 addr, u32 data);
u32 gmac_cr_read(u32 id, u32 addr);

#endif  // _GMAC_H_

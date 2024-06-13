// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  mctp user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _MCTP_USR_API_H_
#define _MCTP_USR_API_H_

#define MCTP_DATA_FUN_TYPE 0x71
#define MCTP_STATISTICS_FUN_TYPE 0x72
#define MCTP_MAX_QUERY_NUM            24

#define MCTP_DEV_NAME "/dev/mctp"
// Maximum timeout interval for reading data, in us.
#define MCTP_TIMEOUT_MAX  (10 * 60 * 1000 *1000)
#define MCTP_TIMEOUT_NONE  (~0U)

#define DEF_PCIE_FMT 0x03
#define DEF_PCIE_TYPE 0x12
#define DEF_PCIE_TC 0x00
#define DEF_PCIE_TD 0x00
#define DEF_PCIE_EP 0x00
#define DEF_PCIE_ATTR 0x00
#define DEF_PCIE_LEN 0x80
#define DEF_PCIE_REQ 0x0000
#define DEF_PCIE_PAD 0x00
#define DEF_PCIE_VDMCODE 0x00
#define DEF_PCIE_MSGCODE 0x7f
#define DEF_PCIE_TARGET 0x0000
#define DEF_PCIE_VDID 0x1AB4

#define DEF_MCTP_VER 0x01
#define DEF_MCTP_DESTEID 0x02
#define DEF_MCTP_SRCEID 0x01
#define DEF_MCTP_TAG 0x00
#define DEF_MCTP_TO 0x00
#define DEF_MCTP_SEQ 0x00
#define DEF_MCTP_EOM 0x01
#define DEF_MCTP_SOM 0x01

#define DEF_MCTPMSG_IC 0x00
#define DEF_MCTPMSG_TYPE 0x01

#define MCTP_MSG_HEAD_LEN 16
#define MCTP_MAX_PAYLOAD_LEN 256
#define MCTP_MIN_PAYLOAD_LEN 4

/* Minimum packet size */
#define MCTP_MIN_PACKET_SIZE (MCTP_MIN_PAYLOAD_LEN + MCTP_MSG_HEAD_LEN)

/* Maximum packet size */
#define MCTP_MAX_PACKET_SIZE (MCTP_MAX_PAYLOAD_LEN + MCTP_MSG_HEAD_LEN)

#define MCTP_RESERVE_PACKET_SIZE 40

#define MCTP_TO_REQUEST 0x0
#define MCTP_TO_RESPONSE 0x1
/* mctp message type  */
#define MCTP_CONTROL 0X00
#define MCTP_PLDM 0X01
#define MCTP_NCSI 0x02
#define MCTP_ETHERNET 0x03
#define MCTP_NVME_MI 0x04
#define MCTP_PCI 0x7e
#define MCTP_IANA 0x7f
/* Maximum PID */
#define MAX_MCTP_PID 65535

#pragma pack(1)

typedef struct {
    pid_t pid;
    unsigned char msg_type;
} msg_type_data_s, *p_msg_type_data;

typedef struct {
    pid_t pid;
    unsigned int process_tag;
    unsigned int reserved;
} process_info;
typedef struct {
    process_info p_info;
    unsigned char msg_type;
    unsigned char eid;
} msg_type_eid_data_s, *p_msg_type_eid_data;

typedef struct {
    unsigned char msg_type;
    unsigned int pid;
    unsigned int process_tag;
    unsigned int queue_cnt;
    int eid;
} mctp_configed_info;

typedef struct {
    mctp_configed_info config_info[MCTP_MAX_QUERY_NUM];
    unsigned int count;
} query_config_info;

typedef struct  _pcie_medium_head {
    unsigned char msgcode;
    unsigned char vdmcode : 4;
    unsigned char pad     : 2;
    unsigned char res5    : 2;
    unsigned short requestid;
    unsigned char  len_l;
    unsigned char len_h : 2;
    unsigned char res4  : 2;
    unsigned char attr  : 2;
    unsigned char ep    : 1;
    unsigned char td    : 1;
    unsigned char res3    : 4;
    unsigned char tc      : 3;
    unsigned char res2    : 1;
    unsigned char type    : 5;
    unsigned char fmt     : 2;
    unsigned char res1    : 1;
} pcie_medium_head, *ppcie_medium_head;

typedef struct  _mctp_transport_head {
    unsigned char tag : 3;      // Identifies packets belonging to a particular message
    unsigned char to  : 1;      // Tag Owner bit
    unsigned char seq : 2;      // Sequence number
    unsigned char eom : 1;      // Identifies last packet in message
    unsigned char som : 1;      // Identifies first packet in message
    unsigned char srceid;       // Logical Address of the message originator
    unsigned char desteid;      // Logical Address of the message target
    unsigned char ver  : 4;
    unsigned char res1 : 4;
} mctp_transport_head, *pmctp_transport_head;

// Format of transmitted PCIe VDM-based MCTP packets
typedef struct  _pcie_vdm_head_tx {
    pcie_medium_head    pciemedium;
    mctp_transport_head mcptransport;
    unsigned short      vdid;
    unsigned short      targetid;
} pcie_vdm_head_tx, *ppcie_vdm_head_tx;

// Format of the received PCIe VDM-based MCTP packet
typedef struct  _pcie_vdm_head_rx {
    unsigned char len_l;
    unsigned char len_h : 2;
    unsigned char res1  : 6;
    unsigned char res2;
    unsigned char type : 5;
    unsigned int  res3 : 3;
    unsigned char res4;
    unsigned char vdmcode : 4;
    unsigned char pad     : 2;
    unsigned char res5    : 2;
    unsigned short      requestid;
    unsigned short      vdid;
    unsigned short      targetid;
    mctp_transport_head mcptransport;
} pcie_vdm_head_rx, *ppcie_vdm_head_rx;

/* Similar to UDP/ICMP/TCP */
typedef struct  _mctp_msg_head {
    unsigned char type : 7;     // Identifies the payload format used for higher level protocols
    unsigned char ic   : 1;     // Integrity Check bit
    unsigned char instance_id : 5;
    unsigned char rsv		  : 1;
    unsigned char d_bit 	  : 1;
    unsigned char rq		  : 1;
    unsigned char com_code;
} mctp_msg_head, *pmctp_msg_head;

struct mctp_statistics {
    // Number of Received Packets
    unsigned long long packets_in;
    unsigned long long packets_out;
    unsigned long long packets_out_reg;
    unsigned long long packets_drop_crc;
    unsigned long long packets_drop_len;
    unsigned long long packets_drop_fifo;
    unsigned long long packets_drop_fifo1;
};
#pragma pack()

#define MCTP_IOC_MAGIC 'm'
#define MCTP_IOC_MAXNR 5
#define MCTP_CMD_CONFIG_TABLE _IOW(MCTP_IOC_MAGIC, 0, msg_type_data_s)
#define MCTP_CMD_REINIT _IO(MCTP_IOC_MAGIC, 1)
#define MCTP_CMD_STATISTICS _IOR(MCTP_IOC_MAGIC, 2, struct mctp_statistics)
#define MCTP_CMD_CONFIG_EID_TABLE _IOW(MCTP_IOC_MAGIC, 3, msg_type_eid_data_s)
#define MCTP_CMD_CONFIG_INFO  _IOR(MCTP_IOC_MAGIC, 4, query_config_info)

#endif // _MCTP_USR_API_H_
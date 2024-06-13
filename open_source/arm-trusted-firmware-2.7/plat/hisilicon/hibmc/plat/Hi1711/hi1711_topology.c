/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hi1711_topology
 * Author: xxx
 * Create: 2022-11-16
 */
#include <arch.h>
#include <platform_def.h>
#include <lib/psci/psci.h>
#include <common/debug.h>
/*
 * The HiKey power domain tree descriptor. The cluster power domains
 * are arranged so that when the PSCI generic code creates the power
 * domain tree, the indices of the CPU power domain nodes it allocates
 * match the linear indices returned by plat_core_pos_by_mpidr().
 */
const unsigned char g_hi1711_power_domain_tree_desc[] = {
    1,                                  /* Number of root nodes */
    PLATFORM_CLUSTER_COUNT,             /* Number of clusters */
    PLATFORM_CORE_COUNT_PER_CLUSTER,    /* Number of children for the first cluster node */
};

/*****************************************************************************************
 * This function returns the HiKey topology tree information.
 ****************************************************************************************/
const unsigned char *plat_get_power_domain_tree_desc(void)
{
    return g_hi1711_power_domain_tree_desc;
}

/*****************************************************************************************
 * This function implements a part of the critical interface between the psci generic layer
 * and the platform that allows the former to query the platform to convert an MPIDR to a
 * unique linear index. An error code (-1) is returned in case the MPIDR is invalid.
 ****************************************************************************************/
int plat_core_pos_by_mpidr(u_register_t mpidr)
{
    unsigned int cluster_id, cpu_id;

    mpidr &= 0x0003FFFF;

    if (mpidr & ~(MPIDR_CLUSTER_MASK | MPIDR_CPU_MASK)) {
        NOTICE("[%s %d] 0x%lx.\n", __func__, __LINE__, mpidr);
        return -1;
    }

    cluster_id = (mpidr >> MPIDR_AFF1_SHIFT) & MPIDR_AFFLVL_MASK;
    cpu_id = (mpidr >> MPIDR_AFF0_SHIFT) & MPIDR_AFFLVL_MASK;

    if (cluster_id >= PLATFORM_CLUSTER_COUNT) {
        NOTICE("[%s %d] 0x%lx.\n", __func__, __LINE__, mpidr);
        return -1;
    }

    /*
     * Validate cpu_id by checking whether it represents a CPU in
     * one of the two clusters present on the platform.
     */
    if (cpu_id >= PLATFORM_CORE_COUNT_PER_CLUSTER) {
        NOTICE("[%s %d] 0x%lx.\n", __func__, __LINE__, mpidr);
        return -1;
    }

    return (cpu_id + (cluster_id * PLATFORM_CORE_COUNT_PER_CLUSTER));
}

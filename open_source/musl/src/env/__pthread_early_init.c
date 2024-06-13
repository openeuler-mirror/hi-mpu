/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: pthread stack size
 * Author: Compiler Team
 * Create: 2023-01-03
*/

#include "pthread_impl.h"
#include <sys/resource.h>

#ifdef __linux__
void __pthread_early_init()
{
    struct rlimit limit = {0};
    int ret = getrlimit (RLIMIT_STACK, &limit);
    if (ret != 0 || limit.rlim_cur == RLIM_INFINITY ||
        limit.rlim_cur < DEFAULT_STACK_SIZE) {
        limit.rlim_cur = DEFAULT_STACK_SIZE;
    }

    __default_stacksize = limit.rlim_cur;
}
#endif
#define _BSD_SOURCE
#include <nl_types.h>
#include <stdint.h>
#include <endian.h>
#include <sys/mman.h>
#include <errno.h>

#define V(p) be32toh(*(uint32_t *)(p))

int catclose (nl_catd catd)
{
    /* Be generous if catalog which failed to be open is used.  */
    if (catd == (nl_catd)-1) {
        errno = EBADF;
        return -1;
    }

	char *map = (char *)catd;
	munmap(map, V(map+8)+20);
	return 0;
}

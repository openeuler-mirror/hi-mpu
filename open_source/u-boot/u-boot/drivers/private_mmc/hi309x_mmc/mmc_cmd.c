#include <asm/io.h>
#include <linux/delay.h>
#include <common.h>
#include "hisi_mmc.h"
#include "hi309x_mmc.h"
#include "malloc.h"
#include <command.h>
#include "fs.h"

#define strtoul(cp, endp, base)     simple_strtoul(cp, endp, base)

unsigned int g_init_flag = 0;

static int do_emmc_init(struct cmd_tbl *cmdtp, int flag, int argc,char * const argv[])
{
    if (emmc_init())
    {
        printf("mmc init failed");
        return -1;
    }

    printf("mmc init success\n");

    g_init_flag = 1;
    return 0;
}

static int do_emmc_switch_area(struct cmd_tbl *cmdtp, int flag, int argc,char * const argv[])
{
    unsigned int index = 0;

    if (g_init_flag == 0)
    {
        emmc_init();
    }

    index = strtoul(argv[1], NULL, 0);
    if(index > 7)
    {
        printf("not emmc %d area\n",index);
        return -1;
    }

    if (mmc_non_dma_change_area(index, 3))
    {
        printf("change area failed\n");
        return -1;
    }

    return 0;
}

static int do_emmc_read(struct cmd_tbl *cmdtp, int flag, int argc,char * const argv[])
{
    unsigned char *buf;
    unsigned int num;
    unsigned int start;

    if (argc != 4)
    {
        printf("input argc error\n");
        return -1;
    }

    if (g_init_flag == 0)
    {
        emmc_init();
    }

    buf = (unsigned char *)strtoul(argv[1], NULL, 0);
    start = strtoul(argv[2], NULL, 0);
    num = strtoul(argv[3], NULL, 0);
    printf("buf=0x%x, start_blk=0x%x , blk_num=0x%x\n", (unsigned int)(uintptr_t)buf, start, num);

    if (mmc_non_dma_read(buf, start, num, 2))
    {
        printf("read data failed\n");
        return -1;
    }

    return 0;
}

static int do_emmc_write(struct cmd_tbl *cmdtp, int flag, int argc,char * const argv[])
{
    unsigned char *buf;
    unsigned int num;
    unsigned int start;

    if (argc != 4)
    {
        printf("[write]input argc error\n");
        return -1;
    }

    if (g_init_flag == 0)
    {
        emmc_init();
    }

    buf = (unsigned char *)strtoul(argv[1], NULL, 0);
    start = strtoul(argv[2], NULL, 0);
    num = strtoul(argv[3], NULL, 0);
    printf("buf=0x%x, start_blk=0x%x , blk_num=0x%x\n", (unsigned int)(uintptr_t)buf, start, num);

    if (mmc_non_dma_write(buf, start, num, 2))
    {
        printf("write data failed\n");
        return -1;
    }

    printf("write success...\n");
    return 0;
}

static int do_emmc_dma_read(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned char *buf;
    unsigned int num;
    unsigned int start;

    if (argc != 4)
    {
        printf("input argc error\n");
        return -1;
    }

    if (g_init_flag == 0)
    {
        emmc_init();
    }

    buf = (unsigned int)strtoul(argv[1], NULL, 0);
    start = strtoul(argv[2], NULL, 0);
    num = strtoul(argv[3], NULL, 0);
    printf("buf=0x%x, start_blk=0x%x , blk_num=0x%x\n", (unsigned int)(uintptr_t)buf, start, num);

    if (mmc_dma_read(buf, start, num))
    {
        printf("dma read data failed\n");
        return -1;
    }

    printf("dma read success...\n");
    return 0;
}

static int do_emmc_dma_write(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned char *buf;
    unsigned int num;
    unsigned int start;

    if (argc != 4)
    {
        printf("[write]input argc error\n");
        return -1;
    }

    if (g_init_flag == 0)
    {
        emmc_init();
    }

    buf = (unsigned char *)strtoul(argv[1], NULL, 0);
    start = strtoul(argv[2], NULL, 0);
    num = strtoul(argv[3], NULL, 0);
    printf("buf=0x%x, start_blk=0x%x , blk_num=0x%x\n", (unsigned int)(uintptr_t)buf, start, num);

    if (mmc_dma_write(buf, start, num))
    {
        printf("dma write data failed\n");
        return -1;
    }

    printf("dma write success...\n");
    return 0;
}

static int do_emmc_get_ex_csd(struct cmd_tbl *cmdtp, int flag, int argc,char * const argv[])
{
    unsigned char buf[512];
    unsigned int index;

    if (argc != 2)
    {
        printf("[get ext csd]input argc error\n");
        return -1;
    }

    if (g_init_flag == 0)
    {
        emmc_init();
    }

    index = strtoul(argv[1], NULL, 0);
    if (index > 512)
    {
        printf("input id(%d) is > 512\n",index);
        return -1;
    }

    if (mmc_non_dma_get_ext_csd(buf))
    {
        printf("get ext csd failed\n");
        return -1;
    }

    printf("buf[%d] = 0x%x\n",index,buf[index]);

    return 0;
}
extern void load_serial_bin_xmoderm(void);
static int do_emmc_update_bin(struct cmd_tbl *cmdtp, int flag, int argc,char * const argv[])
{
    unsigned char *buf;
    unsigned int num;
    unsigned int start;
    char *nv_str = NULL;
    if (argc != 2) {
        printf("input argc error\n");
        return -1;
    }
    load_serial_bin_xmoderm();
    buf = (unsigned char *)(uintptr_t)CONFIG_SYS_LOAD_ADDR;
	nv_str = env_get("loadaddr");
	if (nv_str)
		buf = (unsigned char *)(uintptr_t)hextoul(nv_str, NULL);
    if (g_init_flag == 0) {
        emmc_init();
    }
    num = env_get_hex("filesize", 102400);
    num = num / 512 + 1;
    if (num >= USER_BIN_BLOCK_NUM) {
        printf("input num[%d] error\n", num);
        return -1;
    }
    nv_str = argv[1];
    if (strcmp(nv_str, "bin0") == 0) {
        start = EMMC_BIN_UPDATE_ADDR + 0 * USER_BIN_BLOCK_NUM;
        env_set_hex("bin0size", num);
    } else if (strcmp(nv_str, "bin1") == 0) {
        start = EMMC_BIN_UPDATE_ADDR + 1 * USER_BIN_BLOCK_NUM;
        env_set_hex("bin1size", num);
    } else if (strcmp(nv_str, "bin2") == 0) {
        start = EMMC_BIN_UPDATE_ADDR + 2 * USER_BIN_BLOCK_NUM;
        env_set_hex("bin2size", num);
    } else if (strcmp(nv_str, "bin3") == 0) {
        start = EMMC_BIN_UPDATE_ADDR + 3 * USER_BIN_BLOCK_NUM;
        env_set_hex("bin3size", num);
    }

    if (mmc_non_dma_change_area(0, 3)) { // 分区标号,0:user,1-2:boot,3:RPMB,4-7:GPP
        printf("change area failed\n");
        return -1;
    }
    if (mmc_non_dma_write(buf, start, num, 2))
    {
        printf("write data failed\n");
        return -1;
    }
    printf("update %s [%d blocks] success!\n", nv_str, num);
    return 0;
}

static int do_emmc_erase_bin(struct cmd_tbl *cmdtp, int flag, int argc,char * const argv[])
{
    unsigned int num = 1024;
    unsigned int start;
    char *nv_str = NULL;
    if (argc != 2) {
        printf("input argc error\n");
        return -1;
    }
    if (g_init_flag == 0) {
        emmc_init();
    }
    nv_str = argv[1];
    if (strcmp(nv_str, "bin0") == 0) {
        start = EMMC_BIN_UPDATE_ADDR + 0 * USER_BIN_BLOCK_NUM;
    } else if (strcmp(nv_str, "bin1") == 0) {
        start = EMMC_BIN_UPDATE_ADDR + USER_BIN_BLOCK_NUM;
    } else if (strcmp(nv_str, "bin2") == 0) {
        start = EMMC_BIN_UPDATE_ADDR + 2 * USER_BIN_BLOCK_NUM;
    } else if (strcmp(nv_str, "bin3") == 0) {
        start = EMMC_BIN_UPDATE_ADDR + 3 * USER_BIN_BLOCK_NUM;
    }
    if (mmc_non_dma_change_area(0, 3)) { // 分区标号,0:user,1-2:boot,3:RPMB,4-7:GPP
        printf("change area failed\n");
        return (-1);
    }
    unsigned char *buf = (unsigned char *)malloc(num * 512);
    memset(buf, 0, num * 512);
    if (mmc_non_dma_write(buf, start, num, 2))
    {
        printf("write data failed\n");
        free(buf);
        return (-1);
    }
    free(buf);
    return 0;
}

#define TMP_SIZE 128
#define TMP_END (TMP_SIZE - 32)
#define EXT_CSD_EXTRACT_BOOT_ACK(x)		(((x) >> 6) & 0x1)
#define EXT_CSD_EXTRACT_BOOT_PART(x)		(((x) >> 3) & 0x7)
#define EXT_CSD_EXTRACT_PARTITION_ACCESS(x)	((x) & 0x7)

static void convert_cmd(int argc, char *const argv[], const char *str, char *buf) {
    int len = strlen(str);
    char *end = buf + TMP_END;
    memcpy(buf, str, len);
    buf += len;
    for (int i = 2; i < argc; i++) {
        if (buf > end) {
            break;
        }
        memcpy(buf, argv[i], strlen(argv[i]));
        buf += strlen(argv[i]);
        *buf = ' ';
        buf++;
    }
    *buf = '\0';
}

int do_ext4_size(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    char tempcmd[TMP_SIZE];
    char *buf = tempcmd;
    const char *str = "ext4size private_device ";
    if (strcmp(argv[1], "mmc") == 0) {
        convert_cmd(argc, argv, str, buf);
        run_command(tempcmd, 0);
        return 0;
    }
	return do_size(cmdtp, flag, argc, argv, FS_TYPE_EXT);
}

int do_ext4_write(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    char tempcmd[128];
    char *buf = tempcmd;
    const char *str = "ext4write private_device ";
    if (strcmp(argv[1], "mmc") == 0) {
        convert_cmd(argc, argv, str, buf);
        run_command(tempcmd, 0);
        return 0;
    }
    return do_save(cmdtp, flag, argc, argv, FS_TYPE_EXT);
}

int do_ext4_rm(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    char tempcmd[128];
    char *buf = tempcmd;
    const char *str = "ext4rm private_device ";
    if (strcmp(argv[1], "mmc") == 0) {
        convert_cmd(argc, argv, str, buf);
        run_command(tempcmd, 0);
        return 0;
    }
    return do_rm(cmdtp, flag, argc, argv, FS_TYPE_EXT);
}

int do_ext4_load(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    char tempcmd[128];
    char *buf = tempcmd;
    const char *str = "ext4load private_device ";
    if (strcmp(argv[1], "mmc") == 0) {
        convert_cmd(argc, argv, str, buf);
        run_command(tempcmd, 0);
        return 0;
    }
	return do_load(cmdtp, flag, argc, argv, FS_TYPE_EXT);
}

int do_ext4_ls(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    char tempcmd[128];
    char *buf = tempcmd;
    const char *str = "ext4ls private_device ";
    if (strcmp(argv[1], "mmc") == 0) {
        convert_cmd(argc, argv, str, buf);
        run_command(tempcmd, 0);
        return 0;
    }
	return do_ls(cmdtp, flag, argc, argv, FS_TYPE_EXT);
}

static int do_priv_mmcread(struct cmd_tbl *cmdtp, int flag,
    int argc, char *const argv[]) {
    if (argc != 4) {
        printf("input argc error\n");
        return -1;
    }

    if (g_init_flag == 0) {
        emmc_init();
    }
    unsigned char *buf = (unsigned char *)(uintptr_t)strtoul(argv[1], NULL, 0);
    unsigned int start = strtoul(argv[2], NULL, 0);
    unsigned int num = strtoul(argv[3], NULL, 0);
    mmc_non_dma_read(buf, start, num, 2);
    return 0;
}

static int do_priv_mmcwrite(struct cmd_tbl *cmdtp, int flag,
    int argc, char *const argv[]) {
    if (argc != 4) {
        printf("input argc error\n");
        return -1;
    }

    if (g_init_flag == 0) {
        emmc_init();
    }
    unsigned char *buf = (unsigned char *)strtoul(argv[1], NULL, 0);
    unsigned int start = strtoul(argv[2], NULL, 0);
    unsigned int num = strtoul(argv[3], NULL, 0);
    mmc_non_dma_write(buf, start, num, 2);
    return 0;
}

static int print_partconf_info(const char *varname)
{
    char buffer[512];
    unsigned char ack, access, part;
    (void)mmc_non_dma_get_ext_csd(buffer);
    unsigned char part_config = buffer[EXT_CSD_PART_CONF];
    access = EXT_CSD_EXTRACT_PARTITION_ACCESS(part_config);
    ack = EXT_CSD_EXTRACT_BOOT_ACK(part_config);
    part = EXT_CSD_EXTRACT_BOOT_PART(part_config);
    if(varname)
        env_set_hex(varname, part);
    printf("EXT_CSD[179], PARTITION_CONFIG:\n"
        "BOOT_ACK: 0x%x\n"
        "BOOT_PARTITION_ENABLE: 0x%x\n"
        "PARTITION_ACCESS: 0x%x\n", ack, part, access);
    return 0;
}

static int do_priv_mmcpart(struct cmd_tbl *cmdtp, int flag,
    int argc, char *const argv[]) {
	if (argc != 2 && argc != 3 && argc != 5)
		return CMD_RET_USAGE;

    if (g_init_flag == 0) {
        emmc_init();
    }
    if (argc == 2 || argc == 3) {
        const char *varname = (argc == 3) ? argv[2] : NULL;
        return print_partconf_info(varname);
    }
	// unsigned int ack = dectoul(argv[2], NULL);
	// unsigned int part_num = dectoul(argv[3], NULL);
	unsigned int access = dectoul(argv[4], NULL);
    mmc_non_dma_change_area(access, 2);
    printf("switch to %d area success\n", access);
    return 0;
}

static struct cmd_tbl cmd_mmc[] = {
	U_BOOT_CMD_MKENT(read, 4, 0, do_priv_mmcread, "", ""),
    U_BOOT_CMD_MKENT(write, 4, 0, do_priv_mmcwrite, "", ""),
	U_BOOT_CMD_MKENT(partconf, 5, 0, do_priv_mmcpart, "", ""),
};

static int do_priv_mmcops(struct cmd_tbl *cmdtp, int flag, int argc,
		     char *const argv[])
{
	struct cmd_tbl *cp;

	cp = find_cmd_tbl(argv[1], cmd_mmc, ARRAY_SIZE(cmd_mmc));

	/* Drop the mmc command */
	argc--;
	argv++;

	if (cp == NULL || argc > cp->maxargs)
		return CMD_RET_USAGE;
	if (flag == CMD_FLAG_REPEAT && !cmd_is_repeatable(cp))
		return CMD_RET_SUCCESS;

	return cp->cmd(cmdtp, flag, argc, argv);
}

U_BOOT_CMD(mmc, 6, 1, do_priv_mmcops, 	"MMC sub system",
	"mmc read addr blk# cnt\n"
	"mmc write addr blk# cnt\n"
    "mmc partconf <dev> [[varname] | [<boot_ack> <boot_partition> <partition_access>]]\n"
);

U_BOOT_CMD(
	ext4size,	4,	0,	do_ext4_size,
	"determine a file's size",
	"<interface> <dev[:part]> <filename>\n"
	"    - Find file 'filename' from 'dev' on 'interface'\n"
	"      and determine its size."
);

U_BOOT_CMD(
	ext4rm,	4,	1,	do_ext4_rm,
	"delete a file",
	"<interface> [<dev[:part]>] <filename>\n"
	"    - delete a file from 'dev' on 'interface'"
);

U_BOOT_CMD(ext4ls, 4, 1, do_ext4_ls,
	   "list files in a directory (default /)",
	   "<interface> <dev[:part]> [directory]\n"
	   "    - list files from 'dev' on 'interface' in a 'directory'");

U_BOOT_CMD(ext4load, 7, 0, do_ext4_load,
	   "load binary file from a Ext4 filesystem",
	   "<interface> [<dev[:part]> [addr [filename [bytes [pos]]]]]\n"
	   "    - load binary file 'filename' from 'dev' on 'interface'\n"
	   "      to address 'addr' from ext4 filesystem");

U_BOOT_CMD(ext4write, 7, 1, do_ext4_write,
	   "create a file in the root directory",
	   "<interface> <dev[:part]> <addr> <absolute filename path>\n"
	   "    [sizebytes] [file offset]\n"
	   "    - create a file in / directory");

U_BOOT_CMD(
    mmc_init, 8, 0, do_emmc_init,
    "emmc init",
    "emmc init"
);

U_BOOT_CMD(
    mmc_switch, 10, 0, do_emmc_switch_area,
    "emmc switch area",
    "emmc switch area"
);

U_BOOT_CMD(
    mmc_rd, 6, 0, do_emmc_read,
    "emmc data read",
    "emmc data read"
);

U_BOOT_CMD(
    mmc_wr, 6, 0, do_emmc_write,
    "emmc data write",
    "emmc data write"
);

U_BOOT_CMD(
    mmc_dma_rd, 6, 0, do_emmc_dma_read,
    "emmc dma data read",
    "emmc dma data read"
);

U_BOOT_CMD(
    mmc_dma_wr, 6, 0, do_emmc_dma_write,
    "emmc dma data write",
    "emmc dma data write"
);

U_BOOT_CMD(
    mmc_ecsd, 8, 0, do_emmc_get_ex_csd,
    "emmc get ex-csd",
    "emmc get ex-csd-register"
);

U_BOOT_CMD(
    update, 8, 0, do_emmc_update_bin,
    "update bin0/1/2/3",
    "update bin0/1/2/3"
);

U_BOOT_CMD(
    erase, 8, 0, do_emmc_erase_bin,
    "erase bin0/1/2/3",
    "erase bin0/1/2/3"
);
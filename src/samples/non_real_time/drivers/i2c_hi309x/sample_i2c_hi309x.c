/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2023. All rights reserved.
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define BUFF_SIZE 2
#define ADXL345_SLAVE_ADDR 0x53

int main(int argc, char **argv)
{
    int fd;
    unsigned char w_buf[BUFF_SIZE];
    unsigned char r_buf[BUFF_SIZE];

    fd = open("/dev/i2c-0", O_RDWR);
    if (fd < 0) {
        printf("can't open /dev/i2c-0\n");
        return -1;
    }

    ioctl(fd, I2C_TENBIT, 0);
    ioctl(fd, I2C_SLAVE, ADXL345_SLAVE_ADDR); // 设置从设备地址

    w_buf[0] = 0x80;
    write(fd, w_buf, 1);

    printf("-------------- write end --------------\n");

    read(fd, r_buf, 1);

    printf("read[0] = 0x%x\n", r_buf[0]);

    close(fd);
}
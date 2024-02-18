#include "gpio.h"

int gpio_init() {
    int fd = open(GPIO_DEV_NAME, O_RDONLY);
    if (fd < 0) {
        printf("Unable to open %s: %s", GPIO_DEV_NAME, strerror(errno));
        return -1;
    }

    return fd;
}

void gpio_close(int fd)
{
    close(fd);
}

void gpio_write(int fd, int pin, int value) {
    int ret;

    struct gpiohandle_request rq;
    rq.lineoffsets[0] = pin;
    rq.lines = 1;
    rq.flags = GPIOHANDLE_REQUEST_OUTPUT;

    ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);

    if (ret == -1) {
        printf("Unable to get line handle from ioctl: %s", strerror(errno));
        return;
    }

    struct gpiohandle_data data;
    data.values[0] = value;
    ret = ioctl(rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);

    if (ret == -1) {
        printf("Unable to set line value using ioctl: %s", strerror(errno));
    }

    close(rq.fd);
}



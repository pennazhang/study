#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define MSG(args...) printf(args) 

//函数声明
static int gpio_export(int pin);
static int gpio_unexport(int pin);
static int gpio_direction(char *pinName, int dir);
static int gpio_write(char *pinName, int value);
static int gpio_read(char *pinName);

static int gpio_export(int pin)  
{  
    char buffer[64];  
    int len;  
    int fd;  

    fd = open("/sys/class/gpio/export", O_WRONLY);  
    if (fd < 0) 
	{  
        MSG("Failed to open export for writing!\n");  
        return(-1);  
    }  

    len = snprintf(buffer, sizeof(buffer), "%d", pin);  
    write(fd, buffer, len);
    close(fd);  
    return 0;  
}  

static int gpio_unexport(int pin)  
{  
    char buffer[64];  
    int len;  
    int fd;  

    fd = open("/sys/class/gpio/unexport", O_WRONLY);  
    if (fd < 0) {  
        MSG("Failed to open unexport for writing!\n");  
        return -1;  
    }  

    len = snprintf(buffer, sizeof(buffer), "%d", pin);  
    write(fd, buffer, len); 
    close(fd);  
    return 0;  
} 

//dir: 0-->IN, 1-->OUT
static int gpio_direction(char *pinName, int dir)  
{  
    static const char dir_str[] = "in\0out";  
    char path[64];  
    int fd;  

    snprintf(path, sizeof(path), "/sys/class/gpio/%s/direction", pinName);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) {  
        MSG("Failed to open gpio direction for writing!\n");  
        return -1;  
    }  

    if (write(fd, &dir_str[dir == 0 ? 0 : 3], dir == 0 ? 2 : 3) < 0) {  
        MSG("Failed to set direction!\n");  
        return -1;  
    }  

    close(fd);  
    return 0;  
}  

//value: 0-->LOW, 1-->HIGH
static int gpio_write(char *pinName, int value)  
{  
    static const char values_str[] = "01";  
    char path[64];  
    int fd;  

    snprintf(path, sizeof(path), "/sys/class/gpio/%s/value", pinName);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) {  
        MSG("Failed to open gpio value for writing!\n");  
        return -1;  
    }  

    if (write(fd, &values_str[value == 0 ? 0 : 1], 1) < 0) {  
        MSG("Failed to write value!\n");  
        return -1;  
    }  

    close(fd);  
    return 0;  
}

static int gpio_read(char *pinName)  
{  
    char path[64];  
    char value_str[3];  
    int fd;  

    snprintf(path, sizeof(path), "/sys/class/gpio/%s/value", pinName);  
    fd = open(path, O_RDONLY);  
    if (fd < 0) {  
        MSG("Failed to open gpio value for reading!\n");  
        return -1;  
    }  

    if (read(fd, value_str, 3) < 0) {  
        MSG("Failed to read value!\n");  
        return -1;  
    }  

    close(fd);  
    return (atoi(value_str));
}  


//GPIO1_17
int main()  
{  
	int gpio_fd, ret, i;
	struct pollfd fds[1];
	char buff[10];
	unsigned char cnt = 0;
	int oldValue[7];
	
	//按键引脚初始化
	char *gpioName[] = {"pioC25", "pioC26", "pioC27", "pioC20", "pioC21", "pioC22", "pioC23" };
	int pin[] = {89, 90, 91, 84, 85, 86, 87 };
	
	for (i = 0; i < 7; i++)
	{
		gpio_export(pin[i]);		// GPIO_C25 = S3, PIN = 2 * 32 + 25
		gpio_direction(gpioName[i], 0);  // direction  0: in;  1: out;
		oldValue[i] = gpio_read(gpioName[i]);
		printf("Current GPIO: %s = %d\n", gpioName[i], oldValue[i]);
	}
	while (1)
	{	
		for (i = 0; i < 7; i++)
		{
			usleep(1000);
			int value = gpio_read(gpioName[i]);
			if (value != oldValue[i])
			{
				oldValue[i] = value;
				printf("Current GPIO: %s = %d\n", gpioName[i], oldValue[i]);
			}
		}
	}
	return 0;
} 
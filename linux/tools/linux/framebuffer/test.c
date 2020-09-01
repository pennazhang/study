
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>

//#include "image.h"
//#include "image0.h"
//#include "image1.h"


#define FB_NAME "/dev/fb0"

time_t g_nBeginTimeInSecond;
int g_nLastTimeInSecond = 0;

struct FB_MSG
{
    int fb;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long screensize;
    char *fbp;
};

typedef struct FB_MSG FB;

void print_help()
{
	printf("Usage: fbtest [LastTimeInSecond]\n");
	printf("If no parameters was given, then the test won't stop automatically\n");
	printf("    For example: fbtest 20    --   The test will last for 20 second.\n"); 
}

void open_fb(FB *fb_msg)
{
    int fb;
    fb=open(FB_NAME,O_RDWR);
    if (fb<0)
    {
        printf("Error: Can not open framebuffer device \n");
        exit(1);
    }
     fb_msg->fb=fb;
}

void close_fb(FB *fb_msg)
{
	if (fb_msg->fb != (int)NULL)
	{
		close(fb_msg->fb);
		fb_msg->fb = (int)NULL;
	}
}

int get_fb_msg(FB *fb_msg)
{
    if (ioctl(fb_msg->fb,FBIOGET_FSCREENINFO,&fb_msg->finfo))
    {
        printf("Error: Reading fixed information \n");
        exit(2);
    }
    if (ioctl(fb_msg->fb,FBIOGET_VSCREENINFO,&fb_msg->vinfo))
    {
        printf("Error: Reading variable information \n");
        exit(3);
    }
	printf("res = %d * %d * %d bit\n", fb_msg->vinfo.xres, fb_msg->vinfo.yres, fb_msg->vinfo.bits_per_pixel);
	printf("line_length = %d\n", fb_msg->finfo.line_length);
    return (0);
}

int fb_mmap(FB *fb_msg)
{
    fb_msg->screensize=fb_msg->vinfo.xres * fb_msg->vinfo.yres * fb_msg->vinfo.bits_per_pixel / 8;
    fb_msg->fbp=(char *)mmap(0,fb_msg->screensize,PROT_READ | PROT_WRITE, MAP_SHARED,fb_msg->fb,0);
    if ((int)fb_msg->fbp == -1)
    {
        printf("Error : failed to map framebuffer device to memory. \n");
        exit(4);
    }
    return (0);
}

int fb_ummap(FB *fb_msg)
{
    munmap(fb_msg->fbp,fb_msg->screensize);
    return (0);
}

void draw_picture(const unsigned int *image,FB *fb_msg)
{
    int x,y,count=0;
    long location=0;
    for(x=0;x<fb_msg->vinfo.xres;x++)
    {
    	for(y=fb_msg->vinfo.yres-1;y>=0;y--)
        {
            location = x*(fb_msg->vinfo.bits_per_pixel / 8) + y* fb_msg->finfo.line_length;
            *(fb_msg->fbp+location)=image[count];
            *(fb_msg->fbp+location+1)=image[count+1];
            count=count+2;
        }
    }
}

int clear_screen(char byte0, char byte1, char byte2,FB *fb_msg, int nDelayUs)
{
    int x,y;
    long location=0;
    time_t nCurrentTimeInSecond;
    
    for(x=0;x<fb_msg->vinfo.xres;x++)
    {
        for(y=fb_msg->vinfo.yres-1;y>=0;y--)
        {
        	if (nDelayUs != 0)
        	{
        		nCurrentTimeInSecond = time((time_t*)NULL);
        		if ((nCurrentTimeInSecond - g_nBeginTimeInSecond) > g_nLastTimeInSecond)
        		{
        			return (-1);	
        		}

        		usleep(nDelayUs);
        	}
            location = x*(fb_msg->vinfo.bits_per_pixel / 8) + y* fb_msg->finfo.line_length;
			switch(fb_msg->vinfo.bits_per_pixel)
			{
			case 8:
				*(fb_msg->fbp+location)=byte0;
				break;
			case 16:
				*(fb_msg->fbp+location)=byte0;
	            *(fb_msg->fbp+location+1)=byte1;
				break;
			case 24:
				*(fb_msg->fbp+location)=byte0;
				*(fb_msg->fbp+location+1)=byte1;
				*(fb_msg->fbp+location+2)=byte2;
				break;
			}
        }
    }
	return (0);
}


int main(int argc, char *argv[])
{
	int i;
    FB *fb;
    
    if (argc > 2)
    {
    	print_help();
    	return (-1);
    }
    if (argc == 2)
    {
    	g_nLastTimeInSecond = atoi(argv[1]);
    	if (g_nLastTimeInSecond <= 0)
    	{
    		print_help();
    		return (-1);	
    	}
    }
    else
    {
    	g_nLastTimeInSecond = 0x7fffffff;	
    }
    g_nBeginTimeInSecond = time((time_t*)NULL);
    
    fb=(FB *)malloc(sizeof(FB));
    open_fb(fb);
    get_fb_msg(fb);
    fb_mmap(fb);
	for (i = 5; ; i += 19)
	{
		if (clear_screen((char)(i * 11), (char)(i * 17), (char)(i * 19), fb, 1) != 0)
		{
			break;	
		}
	}
    fb_ummap(fb);
	close_fb(fb);
	free(fb);
	return (0);
}

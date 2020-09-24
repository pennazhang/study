#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;
	
static int count = 0;	
void showPic(int startX, int startY)
{
    long int location = 0;
	int x = startX, y = startY;
	count += 5;
	
    // Figure out where in memory to put the pixel
    for ( y = startY; y < startY + 200; y++ )
        for ( x = startX; x < startX + 200; x++ ) {
			
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                       (y+vinfo.yoffset) * finfo.line_length;

            if ( vinfo.bits_per_pixel == 32 ) {
                *(fbp + location) = 100; // Some blue
                *(fbp + location + 1) = 15+(x-100)/2; // A little green
                *(fbp + location + 2) = 200-(y-100)/5; // A lot of red
                *(fbp + location + 3) = 0; // No transparency
            } else { //assume 16bpp
                int b = count & 0x1F ;
                int g = (x + count) & 0x1F; // A little green
                int r = (y + count)& 0x1F; // A lot of red
                unsigned short int t = r<<11 | g << 5 | b;
                *((unsigned short int*)(fbp + location)) = t;
            }
        }	
}

int main()
{
    int fbfd = 0;
    long int screensize = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        return(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        return(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        return(3);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fbfd, 0);
    if ((int)fbp == -1) {
        printf("Error: failed to map framebuffer device to memory.\n");
        return(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

	showPic(100, 100);
	showPic(vinfo.xres - 300, 100);
	showPic(100, vinfo.yres - 300);
	showPic(vinfo.xres - 300, vinfo.yres - 300);

	munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
	
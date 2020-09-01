#include <stdio.h>
#include <stdlib.h>

typedef  unsigned int UINT32;

const char *MMC_FILE_NAME = "/sys/devices/platform/sunxi-mmc.2/mmc_host/mmc0/mmc0:0001/cid";

UINT32 getMMCVendorID()
{
	char cid[7];
	FILE * file_fd = fopen(MMC_FILE_NAME,"rt");
	unsigned vid;
	int ret;
	
	if(file_fd == NULL)
	{
		printf("Failed to open mmc info file: %s", MMC_FILE_NAME);
		return (0xFFFFFFFF);
	}
	ret = fread(cid, 6, 1, file_fd);
	if (ret <= 0)
	{
		printf("Failed to read mmc info file: %s, ret = %d\n", MMC_FILE_NAME, ret);
		return (0xFFFFFFFF);
	}
	cid[6] = 0;
	vid = strtoul(cid, NULL, 16);
	return vid;
}

int main(int argc, char *argv[])
{
	UINT32 vid = getMMCVendorID();
	printf("vid = 0x%x\n", vid);
	return 0;
}

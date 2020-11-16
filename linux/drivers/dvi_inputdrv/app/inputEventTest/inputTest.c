// version 0.0.0.1
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/input.h>

char dev_name[40];
static int ts_fd = -1;
static struct input_event data;

static int init_device(char *TS_DEV)
{
    if((ts_fd = open(TS_DEV, O_RDONLY)) < 0)
    {
        printf("Error open %s\n\n", TS_DEV);
        return -1;
    }
    return ts_fd;
}

static int test_key()
{
    if(init_device(dev_name) < 0)
        return -1;
    while(1)
    {
        read(ts_fd, &data, sizeof(data));
        if (data.type == EV_KEY)
            printf("  type: EV_KEY, event = %d, value = %d\n",data.code, data.value);
    }
    return 0;
}

static int test_mouse()
{
    if(init_device(dev_name) < 0)
        return -1;
    while(1)
    {
        read(ts_fd, &data, sizeof(data));
        if (data.type == EV_KEY)
        {
            printf("  type = EV_KEY, code = %s, value = %d\n",
                data.code == BTN_LEFT ? "MOUSE_LEFT" :
                data.code == BTN_RIGHT ? "MOUSE_RIGHT" :
                data.code == BTN_MIDDLE ? "MOUSE_MIDDLE" :
                data.code == BTN_SIDE ? "MOUSE_SIDE" :
                "Unkonw", data.value);
        }
        else if(data.type == EV_REL)
        {
            printf("  type = EV_ABS, code = %s, value = %d\n",
                data.code == REL_X ? "ABS_X" :
                data.code == REL_Y ? "ABS_Y" :
                data.code == ABS_WHEEL ? "MOUSE_WHEEL" :
                data.code == ABS_PRESSURE ? "ABS_PRESSURE" :
                "Unkown", data.value);
        }

    }
    return 0;
}

static int test_touch_screen()
{
    if(init_device(dev_name) < 0)
        return -1;
    while(1)
    {
        read(ts_fd, &data, sizeof(data));
        if (data.type == EV_KEY)
        {
            printf("  type: EV_KEY, event = %s, value = %d\n\n",
                data.code == BTN_TOUCH ? "BTN_TOUCH" : "Unkown", data.value);
        }
        else if(data.type == EV_ABS)
        {
            printf("  type: EV_ABS, event = %s, value = %d\n\n",
                data.code == ABS_X ? "ABS_X" :
                data.code == ABS_Y ? "ABS_Y" :
                data.code == ABS_PRESSURE ? "ABS_PRESSURE" :
                "Unkown", data.value);
        }
    }
    return 0;
}


static int test_touch_pancel()
{
    if(init_device(dev_name) < 0)
        return -1;
    while(1)
    {
        read(ts_fd, &data, sizeof(data));
        if (data.type == EV_KEY)
        {
            printf("  type = EV_KEY, code = %s, value = %d\n",
                data.code == BTN_LEFT ? "MOUSE_LEFT" :
                data.code == BTN_RIGHT ? "MOUSE_RIGHT" :
                data.code == BTN_MIDDLE ? "MOUSE_MIDDLE" :
                data.code == BTN_SIDE ? "MOUSE_SIDE" :
                "Unkonw", data.value);
        }
        else if(data.type == EV_REL)
        {
            printf("  type = EV_ABS, code = %s, value = %d\n",
                data.code == REL_X ? "ABS_X" :
                data.code == REL_Y ? "ABS_Y" :
                data.code == ABS_WHEEL ? "MOUSE_WHEEL" :
                data.code == ABS_PRESSURE ? "ABS_PRESSURE" :
                "Unkown", data.value);
        }
    }
    return 0;
}


char *getEventType(unsigned int type)
{
	char *eType[] = {"EV_SYN", "EV_KEY", "EV_REL", "EV_ABS", "EV_MSC", "EV_SW" };
	if (type <= 5)
	{
		return (eType[type]);
	}
	switch (type)
	{
		case EV_LED:   	return "EV_LED";
		case EV_SND:	return "EV_SND (Sound & Alarm)";
		case EV_REP:	return "EV_REP (Repeat)";
		case EV_FF:		return "EV_FF (Force Feedback)";
		case EV_PWR: 	return "EV_PWR (Power)";
		case EV_FF_STATUS:	return "EV_FF_STATUS (Force Feedback Status)";
		default:			return ("Unknown");
	}
}

static int test_others()
{
    if(init_device(dev_name) < 0)
        return -1;
    while(1)
    {
		// The system will block here...
        read(ts_fd, &data, sizeof(data));
		if ((data.type == 0) && (data.code == 0) && (data.value == 0))
		{
			//SYNC message.
		}
		else
		{
			printf("  type: %s (%d), event = %d, value = %d\n", getEventType(data.type), data.type, data.code, data.value);
		}
    }
    return 0;
}

int main()
{
    static int i;
    int eventID;

    printf("Please select device:\n\t0. KeyBoard\n\t1. Mouse\n\t2. TouchScreen\n\t3. TouchPancel\n\t4. Others\n : ");
    scanf("%d",&i);

    printf("\nPlease check /proc/bus/input/devices and enter eventID :  ");
    scanf("%d",&eventID);
    
    if ((eventID > 10) && (eventID < 0))
    {
        printf("Invalid eventID: eventID\n");
        return (-1); 
    }
    sprintf(dev_name, "/dev/input/event%d", eventID);

    switch(i){
        case 0:
            test_key();
            break;
        case 1:
            test_mouse();
            break;
        case 2:
            test_touch_screen();
            break;
        case 3:
            test_touch_pancel();
            break;
        case 4:
            test_others();
            break;
		default:
            printf("Wrong device, Please select again!\n\n");
            break;       
    }
    return 0;
}

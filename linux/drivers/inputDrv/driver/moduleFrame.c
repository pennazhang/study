// Version 0.0.0.1
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/input.h>

#define KERNEL_OK 	0

#define KEY_VALUE_PRESSED	1
#define KEY_VALUE_RELEASED	0

// For long press, it should last at least LONG_PRESS_WAIT_TIME_IN_MS mill-seconds.
#define LONG_PRESS_WAIT_TIME_IN_MS	500	


typedef struct
{
	unsigned process_state;
	int m_wheelRotate;		// 0 clockwise, 1 anti_clockwise
} WHEEL_ROTATE_STATEMACHINE;

const static WHEEL_ROTATE_STATEMACHINE s_wheelRotateMachine[] =
{
	{	0x23102,	1	},	// 10->11->01->00->10	=> anti_clockwise	
	{	0x20132,	0	},	// 10->00->01->11->10	=> clockwise
};


static bool isStateMachinMatch(unsigned process_state, struct rotary_encoder * pEncoder, int *direction)
{
	int i;
	for (i = 0; i < 2; i++)
	{
		if (process_state == pEncoder->pStateMachine[i].process_state >> 4)
		{
			*direction =  pEncoder->pStateMachine[i].m_wheelRotate;
			return (true);
		}
	}
	
	return (false);
}

static irqreturn_t button_irq_handler(int irq, void *param)
{
	struct key_button *button = (struct key_button *)param;
	if (gpio_get_value(button->gpio) == button->last_value)
	{
		return (IRQ_NONE);
	}
//	printk(KERN_INFO "button_irq_handler: gpio = %d, level = %d\n", button->gpio, gpio_get_value(button->gpio));
	// 重新设置一个短按定时器。定时器超时时，会自动调用定时器处理函数来处理。
	mod_timer(&button->timer, jiffies + msecs_to_jiffies(button->poll_ms));
	return IRQ_HANDLED;
}

static irqreturn_t encoder_irq_handler(int irq, void *param)
{
	struct rotary_encoder *pEncoder = (struct rotary_encoder *)param;
	struct keypad_data *pdata = dev_get_drvdata(pEncoder->dev);
	struct input_dev *input = pdata->input;
	int direction;		//clockwise or anti_clockwise
	unsigned current_value;
	
	current_value = (gpio_get_value(pEncoder->gpio[0]) << 1) | gpio_get_value(pEncoder->gpio[1]);
	if (current_value == pEncoder->last_value)
	{
		return IRQ_NONE;
	}
	
	pEncoder->last_value = current_value;
//	printk(KERN_INFO "encoder_irq_handler: gpio = %d,%d, level = %d\n", pEncoder->gpio[0], pEncoder->gpio[1], current_value);
	if (pEncoder->step == 0)
	{
		// step=0表示初始状态，此时我们务必要等到GPIO恢复到初始值（2）,然后才能进行下一步的状态迁移。
		if (isRotateEnd(current_value, pEncoder))
		{
			pEncoder->process_state = current_value;
			pEncoder->step = 1;
		}
	}
	else
	{
		// 正常的状态迁移，不管是否出现抖动，都要继续下去。
		pEncoder->process_state = (pEncoder->process_state << 4) | current_value;
		pEncoder->step++;
	}
	
	if (pEncoder->step == getNormalStep(pEncoder))
	{
		// 旋转过程中经历了4个步骤（GPIO变化了4次），正常情况下应该是要结束了，但也不排除旋转过程中出现抖动的情况。
		// 我们每个周期都不能超过4次操作。第4次变化结束后，我们就将状态初始化一次 (step = 0)。
		pEncoder->step = 0;
		if (isStateMachinMatch(pEncoder->process_state, pEncoder, &direction) == true)
		{
			// We have finished a integrated rotate.
//			printk(KERN_INFO "rotate direction = %d\n", direction);
			input_event(input, EV_MSC, MSC_RAW, direction? pEncoder->event_value[0] : pEncoder->event_value[1]);
			input_sync(input);
			return IRQ_HANDLED;
		}
	}

	return IRQ_HANDLED;
}

//长按定时处理函数，用于检测一个按键是否被一直按下。
static void key_long_press_timer(unsigned long pButton)
{
	struct key_button *button = (struct key_button *)pButton;
	struct keypad_data *pdata = dev_get_drvdata(button->dev);
	struct input_dev *input = pdata->input;
	int current_value = (gpio_get_value(button->gpio));
	
	if (current_value != button->active_low)
	{	
		// Key is pressed down.
//		printk(KERN_INFO "Key is long pressed down. key = %d\n", button->gpio);
		// 长按定时器被触发，且按键处于被按下的状态，因此我们可以认为这是一个标准的长按事件。
		input_event(input, EV_MSC, MSC_RAW, button->long_press_event_value);
		input_sync(input);
	}
}

//短按定时处理函数
static void key_button_timer(unsigned long pButton)
{
	struct key_button *button = (struct key_button *)pButton;
	struct keypad_data *pdata = dev_get_drvdata(button->dev);
	struct input_dev *input = pdata->input;
	int current_value = (gpio_get_value(button->gpio));
	if (current_value == button->last_value)
	{
		return;
	}
	
//	printk(KERN_INFO "pdata = %x, input = %x\n", (unsigned int)pdata, (unsigned int)input);
	button->last_value = current_value;
	if (current_value != button->active_low)
	{
		// Key is pressed down.
//		printk(KERN_INFO "Key is down. key = %d\n", button->gpio);
		// 只要短按定时器被触发，且按键处于按下状态，说明这不是抖动，
		input_report_key(input, button->key_code, KEY_VALUE_PRESSED);
		input_sync(input);
		
		if (button->long_press_event_value != -1)
		{
			// 当检测到短按事件后，我们还需要重新设置一个定时器，来检测长按事件是否被触发。
			mod_timer(&button->longPressTimer, jiffies + msecs_to_jiffies(LONG_PRESS_WAIT_TIME_IN_MS));
		}
	}
	else
	{
		//key is release up.
//		printk(KERN_INFO "Key is up. key = %d\n", button->gpio);
		// stop the longPressTimer.
		//当确定按钮松开后，我们需要取消长按定时器。（长按定时器可能已经超时，也有可能还没有超时）
		del_timer_sync(&button->longPressTimer);
		
		//上报一个按键被release的消息
		input_report_key(input, button->key_code, KEY_VALUE_RELEASED);
		input_sync(input);		
	}
}

static void dumpData(struct keypad_data *pData)
{
	int i;
	struct key_button *pButton;
	struct rotary_encoder* pEncoder;
	printk(KERN_INFO "pdata = %x\n", (unsigned int)pData);
	for (i = 0; i < 3; i++)
	{
		pButton = &pData->button[i];
		printk(KERN_INFO "index: %d\n", i);
		printk(KERN_INFO "button = %x\n", (unsigned int)pButton);	
		printk(KERN_INFO "	dev: %x\n", (unsigned int)pButton->dev);
		printk(KERN_INFO "	desc: %s\n", pButton->desc);
		printk(KERN_INFO "	key_code: %d\n", pButton->key_code);
		printk(KERN_INFO "	gpio: %d\n", pButton->gpio);
		printk(KERN_INFO "	irq: %d\n", pButton->irq);
		printk(KERN_INFO "	active_low: %d\n", pButton->active_low);
		printk(KERN_INFO "	poll_ms: %d\n", pButton->poll_ms);
		printk(KERN_INFO "	long_press_event_value: %d\n", pButton->long_press_event_value);	
	}
		
	printk(KERN_INFO "\n");
	for (i = 0; i < 2; i++)
	{
		pEncoder = &pData->encoder[i];
		printk(KERN_INFO "index: %d\n", i);
		printk(KERN_INFO "encoder = %x\n", (unsigned int)pEncoder);	
		printk(KERN_INFO "	dev: %x\n", (unsigned int)pEncoder->dev);
		printk(KERN_INFO "	desc: %s\n", pEncoder->desc);
		printk(KERN_INFO "	event_value: %d %d\n", pEncoder->event_value[0], pEncoder->event_value[1]);
		printk(KERN_INFO "	gpio: %d %d\n", pEncoder->gpio[0], pEncoder->gpio[1]);
		printk(KERN_INFO "	irq: %d %d\n", pEncoder->irq[0], pEncoder->irq[1]);	
	}	
}

// 从device tree中获取驱动程序所需要的参数
static int keys_parse_dt(struct keypad_data *pData, struct platform_device *pdev, struct input_dev *input)
{

	struct device_node *node = pdev->dev.of_node;
	struct device_node *child_node;
	struct key_button *button;
	struct rotary_encoder *pEncoder;
	int i, k;
	int ret;
	unsigned flags;
	const char *desc;
	int index;
	for (i = 0; i < 3; i++)
	{
		pData->button[i].gpio = -1;	//initialize gpio ID to -1.
	}

//	printk(KERN_INFO "pdata = %x\n", (unsigned int)pData);
	for_each_child_of_node(node, child_node)
	{
		index = -1;
		desc = child_node->name;
		if (strcmp(desc, "rebound-key0") == KERNEL_OK)
		{
			index = 0; 
		}
		else if (strcmp(desc, "rebound-key1") == KERNEL_OK)
		{
			index = 1; 
		}
		else if (strcmp(desc, "rebound-key2") == KERNEL_OK)
		{
			index = 2; 
		}
		
		else if (strcmp(desc, "rotary-encoder0") == KERNEL_OK)
		{
			index = 3; 
		}
		else if (strcmp(desc, "rotary-encoder1") == KERNEL_OK)
		{
			index = 4; 
		}
//		printk(KERN_INFO "index = %d\n", index);	

		if ((index >= 0) && (index <3))
		{
			// It is a GPIO Key button.
			button = &pData->button[index];
			button->dev = &pdev->dev;
//			printk(KERN_INFO "button = %x\n", (unsigned int)button);	
			if (of_property_read_u32(child_node, "key,code", &button->key_code) != KERNEL_OK)
			{
				dev_err(&pdev->dev, "Missing key,code property in the DT for round-key%d.\n", index);
				return (-1);
			}
//			printk(KERN_INFO "key,code = %d\n", button->key_code);	

			button->gpio = of_get_gpio_flags(child_node, 0, &flags);
			if (!gpio_is_valid(button->gpio))
			{
				dev_err(&pdev->dev, "Invalid gpios property in the DT for round-key%d.\n", index);
				return (-1);
			}
			if (flags & OF_GPIO_ACTIVE_LOW)
			{
				button->active_low = 1;			
			}
			else
			{
				button->active_low = 0;
			}
//			printk(KERN_INFO "button->active_low = %d\n", button->active_low);	
			
			desc = of_get_property(child_node, "label", NULL);
			if (desc == NULL)
			{
				dev_err(&pdev->dev, "Invalid label property in the DT for round-key%d.\n", index);
				return (-1);			
			}
			button->desc = desc;
//			printk(KERN_INFO "button->desc = %s\n", button->desc);	
			
			ret = of_property_read_u32(child_node, "poll_ms", &button->poll_ms);
			if (ret != KERNEL_OK)
			{			
				dev_err(&pdev->dev, "Missing poll_ms property in the DT for round-key%d.\n", index);
				return (-1);
			}
			if (button->poll_ms < 1)
			{
				button->poll_ms = 1;
			}
//			printk(KERN_INFO "button->poll_ms = %d\n", button->poll_ms);	

			ret = of_property_read_u32(child_node, "long_press_event_value", &button->long_press_event_value);
			if (ret != KERNEL_OK)
			{			
				dev_err(&pdev->dev, "Missing long_press_event_value property in the DT for round-key%d.\n", index);
				button->long_press_event_value = -1;
			}			
		}
		if ((index == 4) || (index == 3))
		{
			// It is a rotatry encoder.
			pEncoder = &pData->encoder[index - 3];
			pEncoder->dev = &pdev->dev;
			
			desc = of_get_property(child_node, "label", NULL);
			if (desc == NULL)
			{
				dev_err(&pdev->dev, "Invalid label property in the DT for rotary-encoder%d.\n", index - 3);
				return (-1);			
			}
			pEncoder->desc = desc;
//			printk(KERN_INFO "pEncoder->desc = %s\n", pEncoder->desc);	

			for (k=0; k<2; k++)
			{
				pEncoder->gpio[k] = of_get_gpio_flags(child_node, k, &flags);
				printk(KERN_INFO "index = %d, k = %d, gpio = %d\n", index, k, pEncoder->gpio[k]);
				if (!gpio_is_valid(pEncoder->gpio[k]))
				{
					dev_err(&pdev->dev, "Invalid gpios property in the DT for rotary-encoder%d.\n", index - 3);
					return (-1);
				}
			}
			
			if (of_property_read_u32_array(child_node, "event_value", pEncoder->event_value, 2))
			{
				dev_err(&pdev->dev, "Missing event_value property in the DT for rotary-encoder%d.\n", index - 3);
				return -EINVAL;
			}
			printk(KERN_INFO "eventValue = %d %d\n", pEncoder->event_value[0], pEncoder->event_value[1]);
		}
	}
	return 0;
}

static int gpio_set_irq(struct keypad_data *pData, struct platform_device *pdev, struct input_dev *input)
{
	int i, k, ret;
	struct key_button *pButton;
	struct rotary_encoder *pEncoder;
	printk(KERN_INFO "pdata = %x\n", (unsigned int)pData);
	
	set_bit(EV_REP, input->evbit);
	input_set_capability(input, EV_MSC, MSC_RAW); 

	//set interrupt for GPIO key button.
	for (i = 0; i < 3; i++)
	{
		pButton = &pData->button[i];
		if (gpio_is_valid(pButton->gpio))
		{	
			ret = devm_gpio_request_one(&pdev->dev, pButton->gpio, GPIOF_IN, pButton->desc ? pButton->desc : "gpio-keys");
			if (ret < 0)
			{
				dev_err(&pdev->dev, "Failed to request GPIO %d, error %d\n", pButton->gpio, ret);
				return ret;
			}
						
			pButton->irq = gpio_to_irq(pButton->gpio);
			if (pButton->irq < 0)
			{
				ret = pButton->irq;
				dev_err(&pdev->dev, "Unable to get irq number for GPIO %d, error %d\n", pButton->gpio, ret);
				return ret;
			}
		
			pButton->last_value = gpio_get_value(pButton->gpio);
			if (pButton->last_value != pButton->active_low)
			{
				// Key is pressed down.
				printk(KERN_INFO "Current GPIO:%d is pressed down. Please check the active_low setting in DT.\n", pButton->gpio);
			}

			input_set_capability(input, EV_KEY, pButton->key_code);
			
			// 为每个定时器设置回调函数和回调函数的参数
			setup_timer(&pButton->timer, key_button_timer, (unsigned long)pButton);
			setup_timer(&pButton->longPressTimer, key_long_press_timer, (unsigned long)pButton);
			
			// 设置中断处理函数。GPIO管脚上升沿和下降沿都会产生中断。
			ret = devm_request_irq(&pdev->dev, pButton->irq, 
						button_irq_handler,
						IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
						pButton->desc ? pButton->desc : "keys",
						pButton);
			if (ret)
			{
				dev_err(&pdev->dev, "Unable to claim irq %d; error %d\n", pButton->irq, ret);
				return ret;
			}			
		}
		else
		{
			dev_err(&pdev->dev, "Invalid gpios in the DT for round-key%d.\n", i);
			return (-1);		
		}
	}
	
	//set interrupt for GPIO rotary encoder.
	for (i = 0; i < 2; i++)
	{
		pEncoder = &pData->encoder[i];
		for (k = 0; k < 2; k++)
		{
			if (gpio_is_valid(pEncoder->gpio[k]))
			{	
				ret = devm_gpio_request_one(&pdev->dev, pEncoder->gpio[k], GPIOF_IN, pEncoder->desc ? pEncoder->desc : "rotary-encoder");
				if (ret < 0)
				{
					dev_err(&pdev->dev, "Failed to request GPIO %d for encoder:%d, error %d\n", pEncoder->gpio[k], i, ret);
					return ret;
				}
				
				pEncoder->irq[k] = gpio_to_irq(pEncoder->gpio[k]);
				if (pEncoder->irq[k] < 0)
				{
					dev_err(&pdev->dev, "Unable to get irq number for GPIO %d, error %d\n", pEncoder->gpio[k], pEncoder->irq[k]);
					return -1;
				}
				else
				{
					printk(KERN_INFO "irq for gpio %d = %d\n", pEncoder->gpio[k], pEncoder->irq[k]);
				}
			}
			else
			{
				dev_err(&pdev->dev, "Invalid gpios%d:%d in the DT for encoder%d.\n", k, pEncoder->gpio[k], i);
				return (-1);		
			}
		}
		
		// Get current GPIO value.
		pEncoder->last_value = (gpio_get_value(pEncoder->gpio[0]) << 1) | gpio_get_value(pEncoder->gpio[1]);
		
		printk(KERN_INFO "gpio %d = %d\n", pEncoder->gpio[0], gpio_get_value(pEncoder->gpio[0]));
		printk(KERN_INFO "gpio %d = %d\n", pEncoder->gpio[1], gpio_get_value(pEncoder->gpio[1]));
		printk(KERN_INFO "Current encoder:%d value = %d\n", i, pEncoder->last_value);
		
		pEncoder->pStateMachine = (WHEEL_ROTATE_STATEMACHINE *)s_wheelRotateMachine;
		if (!isRotateEnd(pEncoder->last_value, pEncoder))
		{
			// Key is pressed down.
			printk(KERN_INFO "Current encoder:%d is not in steady status. Please check the state-machine setting.\n", i);
		}

		printk(KERN_INFO "irq for GPIO %d:%d = %d:%d\n", pEncoder->gpio[0], pEncoder->gpio[1], pEncoder->irq[0], pEncoder->irq[1]);

		/* Set IRQ for each gpio */
		for (k = 0; k < 2; k++)
		{
			printk(KERN_INFO "request for irq%d:%d %d\n", k, pEncoder->gpio[k], pEncoder->irq[k]);

			// 设置中断处理函数。GPIO管脚上升沿和下降沿都会产生中断。
			ret = devm_request_irq(&pdev->dev, pEncoder->irq[k], 
						encoder_irq_handler,
						IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
						pEncoder->desc ? pEncoder->desc : "encoder",
						pEncoder);
			if (ret)
			{
				printk(KERN_INFO "request for irq %d\n", pEncoder->irq[k]);
				dev_err(&pdev->dev, "Unable to claim irq %d; error %d\n", pEncoder->irq[k], ret);
			}
		
		}
		pEncoder->step = 0;
	}

	return 0;
}

static int keypad_open(struct input_dev *input)
{
	return 0;
}

static void keypad_close(struct input_dev *input)
{
}

static int keypad_probe(struct platform_device *pdev)
{
	int ret;
	int key_num;
	struct keypad_data *pdata;
	struct device *dev = &pdev->dev;
	struct device_node *np = pdev->dev.of_node;
	struct input_dev *input;
	
	if (!pdev->dev.of_node)
	{
		dev_err(&pdev->dev, "no keymap defined\n");
		return -EINVAL;
	}
	
	key_num = of_get_child_count(np);
	if (key_num == 0)
		dev_info(&pdev->dev, "no key defined\n");
	
	input = devm_input_allocate_device(dev);
	if (!input)
	{
		dev_err(dev, "failed to allocate input device\n");
		return -ENOMEM;
	}
	input->name = "gpio-keypad";
	input->phys = "gpio-keys/input0";
	input->dev.parent = dev;
	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;
	input->open = keypad_open;
	input->close = keypad_close;
	//__set_bit(EV_REP, input->evbit);

	/* It will be free when the device driver is unloaded */
	pdata = devm_kzalloc(dev, sizeof(struct keypad_data), GFP_KERNEL);
	if (!pdata)
	{
		dev_err(&pdev->dev, "not enough memory for driver data\n");
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, pdata);
	dev_set_drvdata(&pdev->dev, pdata);

	ret = keys_parse_dt(pdata, pdev, input);
	if (ret)
	{
		dev_err(&pdev->dev, "parse dt faild.\n");
		platform_set_drvdata(pdev, NULL);
		return ret;
	}
//	dumpData(pdata);

	printk(KERN_INFO "pdata = %x, input = %x\n", (unsigned int)pdata, (unsigned int)input);
	ret = gpio_set_irq(pdata, pdev, input);
	if (ret)
	{
		dev_err(&pdev->dev, "set irq failed.\n");
		platform_set_drvdata(pdev, NULL);
		return ret;
	}	
	
	ret = input_register_device(input);
	if (ret)
	{
		dev_err(dev, "Unable to register input device, ret: %d\n", ret);
		return ret;
	}
	pdata->input = input;
	input_set_drvdata(input, pdata);
	
	return 0;
}

static int driver_remove(struct platform_device *pdev)
{
	int i;
	struct keypad_data *pdata = platform_get_drvdata(pdev);
	for (i = 0; i < 3; i++)
	{
		del_timer(&pdata->button[i].timer);
		del_timer(&pdata->button[i].longPressTimer);
	}
	
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id keypad_of_match[] = {
	{ .compatible = "myboard,gpio_output", },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, keypad_of_match);
#endif

static struct platform_driver gpio_output_driver = {
	.probe		= driver_probe,
	.remove		= driver_remove,
	.driver		= {
		.name	= "output-setting",
		.owner  = THIS_MODULE,
		.of_match_table = of_match_ptr(keypad_of_match),
	},
};
module_platform_driver(gpio_output_driver);

MODULE_AUTHOR("Percy Zhang <Percy.Zhang@harman.com>");
MODULE_DESCRIPTION("GPIO output Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:output-module");
cd ~/workspace/imx_m4/SDK_2.8.0_EVK-MIMX8MM/boards/evkmimx8mm/demo_apps/hello_world/armgcc/
./build_debug.sh
cd debug
arm-none-eabi-objcopy -O binary hello_world.elf hello_world.bin
ls -al hello_world.bin

cd ~/workspace/imx_m4/SDK_2.8.0_EVK-MIMX8MM/boards/evkmimx8mm/driver_examples/gpio/led_output/armgcc
./build_debug.sh
cd debug
arm-none-eabi-objcopy -O binary igpio_led_output.elf igpio_led_output.bin
ls -al igpio_led_output.bin

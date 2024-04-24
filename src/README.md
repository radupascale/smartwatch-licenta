## Building using PlatformIO and VSCode
After platformio finishes configuring, try to build then do the following:

1. Change `CONFIG_FREERTOS_HZ=100` to `CONFIG_FREERTOS_HZ=1000` in the generated sdkconfig file.
2. Copy `lv_conf.h` to `.pio/libdeps/lolin_s3_mini`.
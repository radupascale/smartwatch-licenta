Import("env")
import glob
import re

# Use glob to locate the files
config_files = glob.glob('.pio/**/SdFatConfig.h', recursive=True)
driver_files = glob.glob('.pio/**/SdSpiDriver.h', recursive=True)

# Check if the files were found
if not config_files:
    print("SdFatConfig.h not found")
    exit(1)

if not driver_files:
    print("SdSpiDriver.h not found")
    exit(1)

# # For each found config file
for config in config_files:
    with open(config, 'r+') as f:
        content = f.read()
        content_new = re.sub(r'#define ENABLE_DEDICATED_SPI .*', '#define ENABLE_DEDICATED_SPI 0', content)
        f.seek(0)
        f.write(content_new)
        f.truncate()

# For each found driver file
for driver in driver_files:
    with open(driver, 'r+') as f:
        content = f.read()
        content_new = re.sub(r'const uint8_t SHARED_SPI .*', 'const uint8_t SHARED_SPI = 1;', content)
        f.seek(0)
        f.write(content_new)
        f.truncate()
Import("env")

import os
import shutil

"""
_summary_ Copy lv_conf.h to .pio/libdeps/lolin_s3_mini as required by lvlg
"""
lv_conf_path = "./lv_conf.h"
copy_path = "./.pio/libdeps/lolin_s3_mini"

#print pwd
print(f"Current directory: {os.getcwd()}")

if not os.path.exists(lv_conf_path):
    print("lv_conf.h not found in ../src/")
    exit(-1)

if not os.path.exists(copy_path):
    print(".pio/libdeps/lolin_s3_mini not found")
    exit(-1)

# copy lv_conf.h to .pio/libdeps/lolin_s3_mini
shutil.copy(lv_conf_path, copy_path)
print(f"lv_conf.h copied to {copy_path}")
    

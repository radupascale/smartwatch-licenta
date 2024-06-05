# I found this script which does what I want: app_main is guarded in main.cpp
# during testing
# https://community.platformio.org/t/unit-testing-multiple-definition-of-loop-error/17119/4


from SCons.Script import COMMAND_LINE_TARGETS

Import("env")

def add_build_flags():
    if "__test" in COMMAND_LINE_TARGETS:
        env.Append(CCFLAGS=["-DUNIT_TEST=1"])

add_build_flags()
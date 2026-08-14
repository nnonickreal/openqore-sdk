cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := es7243e.o

ccflags-y += \
    -Iplatform/hal \
    -Iplatform/cmsis \
    -Iutils/cqueue \
    -Iservices/nvrecord

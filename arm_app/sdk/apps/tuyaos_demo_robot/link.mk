# 当前文件所在目录
LOCAL_PATH := $(call my-dir)

-include ./../../build/build_param
-include $(TUYAOS_BUILD_PATH)/platform.mk

LIBS_DIR := $(ROOT_DIR)/libs
DSP_LIB_DIR := $(ROOT_DIR)/output/bsp/bsp_sdk/media-sdk
BSP_LIB_DIR := $(ROOT_DIR)/output/bsp/bsp_sdk/lib

#app & OS
LIB_LIST := $(LIBS_DIR)/*.a

#dsp
DSP_LIB_FILE = $(wildcard $(DSP_LIB_DIR)/*.a)
ifneq ($(DSP_LIB_FILE), )
	LIB_LIST += $(DSP_LIB_DIR)/*.a
endif

#bsp
BSP_LIB_FILE = $(wildcard $(BSP_LIB_DIR)/*.a)
ifneq ($(BSP_LIB_FILE), )
	LIB_LIST += $(BSP_LIB_DIR)/*.a
endif


#general
LIB_LIST += -lpthread -ldl -lm -lstdc++ -lrt -lcrypt

LDFLAGS := $(TUYA_PLATFORM_CFLAGS)
LDFLAGS += -Os -ffunction-sections -Wl,--gc-sections,--no-whole-archive,--no-export-dynamic

EXE_DIR := ./../../output/$(TARGET_PLATFORM)_$(PROJECT_NAME)/firmware

tuyaos_demo_robot:
	@-mkdir -p $(EXE_DIR)
	$(CC) $(LDFLAGS) -Xlinker "-(" $(LIB_LIST)  -Xlinker "-)" -o $(EXE_DIR)/tuyaos_demo_robot
	
.PHONY:tuyaos_demo_robot

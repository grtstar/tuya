# 当前文件所在目录
LOCAL_PATH := $(call my-dir)

#---------------------------------------

# 清除 LOCAL_xxx 变量
include $(CLEAR_VARS)

# 当前模块名
LOCAL_MODULE := $(notdir $(LOCAL_PATH))

# 模块对外头文件（只能是目录）
LOCAL_TUYA_SDK_INC +=

# 模块对外CFLAGS：其他组件编译时可感知到
LOCAL_TUYA_SDK_CFLAGS := -DUSER_SW_VER=\"$(APP_VER)\" -DAPP_BIN_NAME=\"$(APP_NAME)\"

# 模块源代码
LOCAL_SRC_FILES += $(shell find $(LOCAL_PATH)/application_components -name "*.c" -o -name "*.cpp" -o -name "*.cc")

# 模块内部CFLAGS：仅供本组件使用
APPS_INC_ALL_SUBDIRS += $(shell find $(LOCAL_PATH)/application_components -type d)
LOCAL_CFLAGS := $(addprefix -I,$(APPS_INC_ALL_SUBDIRS))

LOCAL_CFLAGS += $(addprefix -I,$(APPS_INC_ALL_SUBDIRS))

# 全局变量赋值
TUYA_SDK_INC += $(LOCAL_TUYA_SDK_INC)  # 此行勿修改
TUYA_SDK_CFLAGS += $(LOCAL_TUYA_SDK_CFLAGS)  # 此行勿修改


# 生成静态库
include $(BUILD_STATIC_LIBRARY)

# 生成动态库
include $(BUILD_SHARED_LIBRARY)

# 导出编译详情
include $(OUT_COMPILE_INFO)

#---------------------------------------
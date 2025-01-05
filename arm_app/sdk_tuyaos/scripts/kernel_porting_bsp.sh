#!/bin/sh
#
# ******************** for bsp import, use dlopen template *********************
#
# ==============================================================================
# 1，此脚本用于快速更新开发环境kernel版本，适用于使用tuyaos_auto_tester导入的开发环境
# 2，如果要更新kernel模板，增加下能力，修改tuyaos_kernel.config即可
# 3，仅更新头文件，init和utilities组件，差异需要手动对比更新
# 4，更新代码需要手动提交
# ------------------------------------------------------------------------------
# 入参说明：
#
# $1 - kernel 版本，如： 1.0.0，此参数可选，如果不提供，则会更新到最新的kernel版本，如
#      果不提供，则$2必须手动选择（脚本能力限制）
# $2 - 开发环境名称，如： bk7231n，此参数可选，如果不提供，则会列出vendor下目录供选择
#
# ------------------------------------------------------------------------------
# 使用示例：
# ./kernel_porting.sh 
# 或者
# ./kernel_porting.sh 1.0.0
# 或者
# ./kernel_porting.sh 1.0.0 bk7231n
# ------------------------------------------------------------------------------

print_not_null()
{
    # $1 为空，返回错误
    if [ x"$1" = x"" ]; then
        return 1
    fi

    echo "$1"
}

set -e
cd `dirname $0`
CURRENT_DIR=$(pwd)
ROOT_DIR=$CURRENT_DIR/..
TARGET_PLATFORM_KERNEL_VERSION=`print_not_null $1 || echo latest`
TARGET_PLATFORM=`print_not_null $2 || bash ./get_sub_dir.sh ../vendor`
TARGET_PLATFORM_PATH=$ROOT_DIR/vendor/$TARGET_PLATFORM
TEMPLATE_PATH=$TARGET_PLATFORM_PATH/tuyaos/tuyaos_kernel.config
KERNEL_PATH=$TARGET_PLATFORM_PATH/tuyaos/tuyaos_adapter/
echo TARGET_PLATFORM=$TARGET_PLATFORM
echo TARGET_PLATFORM=$TARGET_PLATFORM_PATH
echo CURRENT_DIR=$CURRENT_DIR
echo ROOT_DIR=$ROOT_DIR

check_kernel_env()
{
    if [ ! -f "$TEMPLATE_PATH" ]; then
        echo kernel template not existed at $TEMPLATE_PATH...
        echo you can copy a kernel template from other vendor...

        exit 1
    fi

    # if [ -e "$KERNEL_PATH" ]; then
    #     echo this vendor $KERNEL_PATH already existed...
    #     echo please backup and try again...
        
    #     exit 2
    # fi
}



check_kernel_env
./kernel_update.sh $TARGET_PLATFORM_KERNEL_VERSION $TARGET_PLATFORM
./porting/kernel_porting.py $TARGET_PLATFORM_PATH $TARGET_PLATFORM bsp
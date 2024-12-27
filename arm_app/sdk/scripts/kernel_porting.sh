#!/bin/sh

# ==============================================================================
# 1，此脚本用于快速更新开发环境kernel版本，适用于使用tuyaos_auto_tester导入的开发环境
# 2，如果要更新kernel模板，增加下能力，修改tuyaos_kernel.config即可
# 3，仅更新头文件，init和utilities组件，差异需要手动对比更新
# 4，更新代码需要手动提交
# ------------------------------------------------------------------------------
# 入参说明：
#
#   -v version: kernel 版本，如： 1.0.0，此参数可选，如果不提供，则会更新到最新的kernel版本
#   -p platform: 开发环境名称，如： bk7231n，此参数可选，如果不提供，则会列出vendor下目录供选择
#
# ------------------------------------------------------------------------------
# 使用示例：
# ./kernel_porting.sh
# 或者
# ./kernel_porting.sh 1.0.0
# 或者
# ./kernel_porting.sh 1.0.0 bk7231n
# ------------------------------------------------------------------------------

set -e

print_not_null()
{
    # $1 为空，返回错误
    if [ x"$1" = x"" ]; then
        return 1
    fi

    echo "$1"
}

help_exec()
{
    NOTE="
Usage: $OPEN_BUILD COMMAND [OPTS]...

Options:
    -v version: kernel 版本，如： 1.0.0，此参数可选，如果不提供，则会更新到最新的kernel版本
    -p platform: 开发环境名称，如： bk7231n，此参数可选，如果不提供，则会列出vendor下目录供选择
"
    echo "$NOTE"
}

while getopts "v:p:" opt; do
    case $opt in
        v)
            KERNEL_VERSION=$OPTARG
            ;;
        p)
            TARGET_PLATFORM=$OPTARG
            ;;
        ?)
            echo "Unknow: $opt"
            help_exec
            exit 1
            ;;
    esac
done

cd `dirname $0`
CURRENT_DIR=$(pwd)
ROOT_DIR=$CURRENT_DIR/..
TARGET_PLATFORM_KERNEL_VERSION=`print_not_null $KERNEL_VERSION || echo latest`
TARGET_PLATFORM=`print_not_null $TARGET_PLATFORM || bash ./get_sub_dir.sh ../vendor`
TARGET_PLATFORM_PATH=$ROOT_DIR/vendor/$TARGET_PLATFORM
TEMPLATE_PATH=$TARGET_PLATFORM_PATH/tuyaos/tuyaos_kernel.config
KERNEL_PATH=$TARGET_PLATFORM_PATH/tuyaos/tuyaos_adapter/
echo TARGET_PLATFORM=$TARGET_PLATFORM
echo TARGET_PLATFORM_KERNEL_VERSION=$TARGET_PLATFORM_KERNEL_VERSION
echo TARGET_PLATFORM_PATH=$TARGET_PLATFORM_PATH
echo CURRENT_DIR=$CURRENT_DIR
echo ROOT_DIR=$ROOT_DIR


./kernel_update.sh $TARGET_PLATFORM_KERNEL_VERSION $TARGET_PLATFORM

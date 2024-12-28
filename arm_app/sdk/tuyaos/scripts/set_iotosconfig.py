#!/usr/bin/env python3
# -*- coding: utf-8 -*-
##
# @file set_iotosconfig.py
# @brief 解析yaml文件，查找组件和工具链目录下是否存在IoTOSconfig文件，
#        如果存在则source到一个总IoTOSconfig文件中
# @author huatuo
# @version 1.0.0
# @date 2021-10-10


import os
import sys
from ruamel import yaml  # pip3 install ruamel.yaml

MAKE_YAML = "./make.yaml" if len(sys.argv)<2 else sys.argv[1]
OSCONFIG = "./build/IoTOSconfig" if len(sys.argv)<3 else sys.argv[2]
PLATFORM = None if len(sys.argv)<4 else sys.argv[3]


def set_iotosconfig(make_yaml, osconfig, platform):
    f = open(make_yaml, "r")
    yaml_res = yaml.load(f.read(), yaml.RoundTripLoader)
    f.close()

    if 'dependencies' not in yaml_res.keys():
        return False

    dependencies = yaml_res['dependencies']

    context = 'mainmenu "Tuya IoT SDK Configuration"\n'
    toolchain_context = ""

    for item_key in dependencies.keys():  # components/adapter/vendor
        item = dependencies[item_key]
        for block in item:
            if "Component" in block.keys():
                d = block['Component']
                if ('name' not in d.keys()) or ('locater' not in d.keys()):
                    continue
                name = d['name']
                locater = d['locater']
                config_path = os.path.join(locater, name, 'IoTOSconfig')
                if os.path.exists(config_path):
                    context += ('source '+config_path+'\n')
            elif "Toolchain" in block.keys():
                d = block['Toolchain']
                if ('name' not in d.keys()) or ('locater' not in d.keys()):
                    continue
                if platform != d['name']:
                    continue
                name = d['name']
                locater = d['locater']
                config_path = os.path.join(locater, name, 'IoTOSconfig')
                if os.path.exists(config_path):
                    toolchain_context = ('source '+config_path+'\n')
            else:
                continue

    context += toolchain_context

    with open(osconfig, 'w') as f:
        f.write(context)

    return True


def main():
    if not os.path.exists(MAKE_YAML):
        print("path error: ", MAKE_YAML)
        exit()
    config_dir = os.path.dirname(OSCONFIG)
    if not os.path.exists(config_dir):
        os.makedirs(config_dir)

    if PLATFORM == None:
        print("toolchain is None ...")
        exit()

    set_iotosconfig(MAKE_YAML, OSCONFIG, PLATFORM)

    pass


if __name__ == '__main__':
    main()
    pass

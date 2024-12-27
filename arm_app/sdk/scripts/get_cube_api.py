#!/usr/bin/env python3
# -*- coding=utf-8 -*-
##
# @file get_cube_api.py
# @brief 用于解析预处理后的文件（.i）
#        收集atop接口的名字和版本信息
#        以json格式写入到输出文件中
#        如果存在append_json
#        则将结果合并
# @author huatuo
# @version 1.0.0
# @date 2024-01-16


import os
import sys
import re
import json


def checkout_atop_file(file_path, api_list):
    if not os.path.exists(file_path):
        return
    with open(file_path, encoding='utf-8') as f:
        contents = f.read()
    func_list = [
        "__iot_common_post",
        "__iot_common_post_sesssion",
        "httpc_common_post",
        "httpc_common_post_no_remalloc",
        "iot_httpc_common_post",
        "iot_httpc_common_post_simple",
        "iot_httpc_common_post_no_remalloc",
    ]
    for func in func_list:
        pattern = f'(?<=\\b{func}\\()' \
                  + '\\s*[\\w,>-]*?\\s*"[\\w\\.]*?",\\s*"\\d\\.\\d",'
        result = re.findall(pattern, contents, re.S)
        pattern = r'(?<=")[\w\.-]*?(?=",)'
        if len(result):
            for r in result:
                ans = re.findall(pattern, r, re.S)
                if len(ans) != 2:
                    continue
                api = {
                    "apiName": ans[0].strip(),
                    "apiVersion": ans[1].strip(),
                }
                if api in api_list:
                    continue
                api_list.append(api)
    pass


def checkout_cube_dir(dir, api_list):
    if not os.path.exists(dir):
        return
    for root, _, files in os.walk(dir):
        for f in files:
            if not f.endswith(".c.i"):
                continue
            checkout_atop_file(os.path.join(root, f), api_list)
    pass


def main(inm_dir, output_file, append_file):
    cube_api_list = []
    if (append_file is not None) and (os.path.exists(append_file)):
        with open(append_file, 'r', encoding='utf-8') as f:
            cube_api_list = json.load(f)
    checkout_cube_dir(inm_dir, cube_api_list)
    if len(cube_api_list) == 0:
        return
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    json_str = json.dumps(cube_api_list, indent=4, ensure_ascii=False)
    with open(output_file, 'w') as f:
        f.write(json_str)
    pass


if __name__ == "__main__":
    inm_dir = "./output" \
        if len(sys.argv) < 2 else sys.argv[1]
    output_file = "./output/cube_api.json" \
        if len(sys.argv) < 3 else sys.argv[2]
    append_file = None \
        if len(sys.argv) < 4 else sys.argv[3]
    main(inm_dir, output_file, append_file)

#!/usr/bin/python3
# -*- coding:utf-8 -*-

# ==============================================================================
# 将schema文件生成字符串
# ------------------------------------------------------------------------------
# 入参说明：
#
# $1 - schema json文件
#
# ------------------------------------------------------------------------------
# 使用示例：
#
# ./schema_json_to_string.py Debugfile_3.10.0_20240311.json
# 上面命令敲了之后,当前目录会生成Debugfile_3.10.0_20240311.json.c,可以直接使用g_schema_dps变量
#
# ------------------------------------------------------------------------------

import os
import sys
import json

def main():
    if not os.path.exists(sys.argv[1]):
        print(sys.argv[1] + "--------not exist")
        return

    with open(sys.argv[1], 'r', encoding='utf-8') as f_schema_json:
        schema_data = json.load(f_schema_json)

        dp_data = []
        for dp in schema_data['Dp_Data']:
            d = {
               'id': dp['id'],
               'type': dp['type'],
               'mode': dp['mode']
            }
            if dp.get('passive'):
                d['passive'] = dp['passive']
            if dp.get('trigger'):
                d['trigger'] = dp['trigger']
            if dp.get('route'):
                d['route'] = dp['route']

            prop = dp.get('property')
            if prop:
                p = {
                    'type': prop['type']
                }
                if p['type'] == 'string':
                    p['maxlen'] = prop['maxlen']
                elif p['type'] == 'bitmap':
                    p['maxlen'] = prop['maxlen']
                elif p['type'] == 'enum':
                    p['range'] = prop['range']
                elif p['type'] == 'value':
                    p['min'] = prop['min']
                    p['max'] = prop['max']
                    p['scale'] = prop['scale']
                    p['step'] = prop['step']

                d['property'] = p

            dp_data.append(d)

        schema_id = 'const char * g_schema_id = "' + schema_data['Pro_Key'] + '";\n';
        schema_dps = 'const char * g_schema_dps = ' + json.dumps(json.dumps(dp_data, separators=(',',':'))) + ';'

        with open(sys.argv[1] + '.c', 'w+', encoding='utf-8') as f_schema_str:
            f_schema_str.write(schema_id)
            f_schema_str.write(schema_dps)

if __name__ == "__main__":
   main()

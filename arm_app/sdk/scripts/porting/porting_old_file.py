#!/usr/bin/python3
# -*- coding:utf-8 -*-

##
# @file gen_import_parse_file.py
# @brief 解析现有文件，提前函数名、函数注释、函数实现、函数返回值等信息。
# @version 1.0.0
# @date 2022-04-10

import os
import sys
import re


# 用户兹定于区域开始和结束标记，用于新增开发者需要的其他头文件、各种类型、枚举、宏、常量、全局变量、内部函数等
udf_flags_begin = "// --- BEGIN: user defines and implements ---\n"
udf_flags_end   = "// --- END: user defines and implements ---" 

# 函数体开始和结束标记，用于新增开发者自己的函数实现
func_body_flags_begin = "// --- BEGIN: user implements ---"
func_body_flags_end   = "// --- END: user implements ---"

class parse_old_file(object):
    def __init__(self, path) -> None:
        self.path = path

    def __load_udfs(self, body):
        includes = re.findall("%s(.+)%s"%(udf_flags_begin,udf_flags_end), body, flags=re.DOTALL)
        #print(includes)
        return includes

    def __paser_func_return(self, raw_return):
        tmp = re.sub("%s"%udf_flags_begin, "", raw_return)
        tmp = re.sub("%s"%udf_flags_end, "", tmp)
        tmp = re.sub("\n*", "", tmp)
        tmp = re.sub("\s*", "", tmp)
        return tmp

    def __load_funcs(self, body):
        funcs = []
        #body = re.sub("\/\*\*.+?file.+?\*\/", "", body, flags=re.DOTALL) # 清除函数注释
        #body = re.sub("\/\*\*.*?\*\/", "", body, flags=re.DOTALL) # 清除函数注释
        body = re.sub("%s.+%s"%(udf_flags_begin,udf_flags_end), "", body, flags=re.DOTALL) # 清除udf区域
        
        #print(body)

        # 提取funcs
        funcs_list = re.split("\/\*\*.+?\*\/", body, flags=re.DOTALL)
        #funcs_list = re.findall("\/\*\*.+?\*\/.*?\(.*?\).*?\{.*?\}", body, flags=re.DOTALL)
        #print(funcs_list)
        for func in funcs_list:
            #print("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb")
            #print(func)

            # 有一些没有函数定义的文件，需要忽略掉
            hastkl = re.findall("tkl", func)
            if not hastkl:
                continue
            

            fhead = re.findall("(.+?tkl.+?\(.+?\))", func, flags=re.DOTALL)
            freturn = re.findall("(.+?\**?)tkl.+?\(.+?\)", func, flags=re.DOTALL) #返回值处理的时候，需要考虑一些写法方面的不一致，比如VOID_T* func和VOID_T *func
            fname = re.findall(".+?(tkl.+?)\s*\(.+\)", func, flags=re.DOTALL)
            fname = re.sub(" ", "", fname[0])
            fname = re.sub("\t", "", fname)
            fbody = re.findall("%s.+?%s"%(func_body_flags_begin,func_body_flags_end), func, flags=re.DOTALL)
            if not fbody:
                #print("xxxxxxxxxxxxxxxxxxxxxxxxxxxx")
                fbody = re.findall("{(.*?)}", func, flags=re.DOTALL)

            #print(fname)
            #print(fhead)
            #print(fbody)
            #print(freturn[0])
            #print("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
            f = {}
            f['return'] = self.__paser_func_return(freturn[0])
            f['name'] = fname
            f['head'] = fhead[0]
            f['body'] = fbody[0]
            funcs.append(f)

        return funcs                    
        
    def __load_x_udfs(self, body):
        udfs = self.__load_udfs(body) # 尝试解析，解析不出来再处理
        if not udfs:
            pass
        
        return udfs

    def __load_x_funcs(self, body):
        funcs = self.__load_funcs(body) # 尝试解析，解析不出来再处理
        if not funcs:
            pass

        return funcs
    
    def load_file(self):
        if not os.path.exists(self.path):
            return 

        file = {}
        file['funcs'] = {}
        print(self.path)
        c = open(self.path, encoding='utf-8')
        s = c.read()

        #print(s)
        # 非自动化生成的，自行开发的，需要尽量加载内容进来重新构建一个自动化生成的文件
        udfs = self.__load_x_udfs(s)
        funcs = self.__load_x_funcs(s)

        file['udfs'] = udfs
        file['funcs'] = funcs

        # debug, 查看文件函数信息
        #print(file)        
        return file
    
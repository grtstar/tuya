#!/usr/bin/python3
# -*- coding:utf-8 -*-

##
# @file gen_import_file.py
# @brief 生成导入文件相关的信息，包括文件头部，文件函数，文件类型定义等信息。
# @version 1.0.0
# @date 2022-04-10
import re

class parse_new_file(object):
    def __init__(self, path) -> None:
        self.path = path
    
    def __paser_func_return(self, raw_return):
        tmp = re.sub("\s*", "", raw_return)
        return tmp

    def load_file(self):
        file = {}
        file['funcs'] = []
        c = open(self.path, encoding="utf-8")
        s = c.read()
        body = re.findall("#ifdef.+cplusplus.+extern.+#endif(.+)#ifdef.+__cplusplus", s, flags=re.DOTALL)
        if body:
            funcs = body[0]
            #print(funcs)

            # 排除一些干扰提取API的信息
            funcs = re.sub("\/\*\*.+?\*\/.+?typedef.+?\(.*?\);", "", funcs, flags=re.DOTALL) # 函数指针
            funcs = re.sub("typedef.+?;", "", funcs) # 类型重定义
            funcs = re.sub("typedef.+{.+}.+?;", "", funcs, flags=re.DOTALL) # 枚举，结构体定义
            funcs = re.sub("#include.+\".+\"", "", funcs) # 内部头文件
            funcs = re.sub("#include.+<.+>", "", funcs) # 库头文件
            funcs = re.sub("\/\*\*.+?\*\/.+?#define.+?TKL_ENTER_CRITICAL.+?tkl_system_enter_critical\(.*?\)", "", funcs, flags=re.DOTALL) # 注意，此行仅为处理system里的TKL_ENTER_CRITICAL,TKL_EXIT_CRITICAL宏
            funcs = re.sub("\/\*\*.+?\*\/.+?#define.+?TKL_EXIT_CRITICAL.+?tkl_system_exit_critical\(.*?\)", "", funcs, flags=re.DOTALL) # 注意，此行仅为处理system里的TKL_ENTER_CRITICAL,TKL_EXIT_CRITICAL宏
            funcs = re.sub("#define.+\n", "", funcs) # 无注释单行宏

            # 提取API
            funcs_list = re.findall("\/\*\*.+?\*\/.+?\(.*?\);", funcs, flags=re.DOTALL)

            # 构造API
            if funcs_list:
                for f in funcs_list:
                    #print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
                    #print(f)
                    fname = re.findall("\*\/.+?(tkl.+)\s*\(.+?\);", f, flags=re.DOTALL)
                    fname = re.sub(" ", "", fname[0])
                    fname = re.sub("\t", "", fname)
                    rt = re.findall("\*\/\s*\n(.+?\**?)tkl", f, flags=re.DOTALL)
                    func = {}
                    func['return'] = self.__paser_func_return(rt[0])
                    func['name'] = fname
                    func['head'] = re.sub("\);", ")", f)
                    func['isnew'] = True
                    file['funcs'].append(func)
                    #print(func['name'])
                    #print(func['head'])
                    #print("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy")
            # debug, 查看文件函数信息
            #print(file)
            file['udfs'] = []
            return file

if __name__ == "__main__":
    gat = autotest_generate_file()
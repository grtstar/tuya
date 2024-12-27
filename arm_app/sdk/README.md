#类型开发包引导
[扫地机开发框架](https://developer.tuya.com/cn/docs/iot-device-dev/robot-vacuum-dev?id=Kd934lbvnavbk)
里面包含
[类型开发包内容清单]
├── hardware                            硬件资料
├── pc                                  PC 端开发与调试工具
├── software                            软件资料
│   ├── packages                        软件包压缩文件
│   ├── SDKInformation.json             类型开发包参数信息，修改无效
│   └── TuyaOS                          代码与库文件
│       ├── apps                        产品目录
│       │   └── tuyaos_demo_robot       Demo 演示产品
│       ├── build                       编译配置，修改无效
│       ├── build_app.sh                产品编译脚本
│       ├── CHANGELOG.md                修订变更记录
│       ├── docs                        文档目录
│       ├── include                     头文件
│       ├── libs                        SDK 库文件
│       ├── LICENSE                     证书
│       ├── Makefile                    Makefile
│       ├── prepare_app.sh              下载编译环境脚本
│       ├── README.md                   说明文档
│       ├── scripts                     编译脚本
│       └── vendor                      芯片平台相关文件与接口
│	    ├── docs      ## BSP 一些通用文档
│	    ├── toolchain ## 平台工具链
│           └── tuyaos    ## vendor 编译组目录
│		├── build ## vendor 编译入口
│    		├── include         ## tuyao_iot_config.h 目录
│    		├── scripts         ## 产生 libtuyaos_adapter_custom.so 的脚本
│    		├── tuyaos_adapter   ## TKL 功能组件代码
│    		│   ├── include   ## TKL 头文件
│   		│   ├── src    ## 需适配的 TKL 接口文件
│    		├── build.conf   ## BSP 依赖的第三方库下载地址
│    		├── build_path   ## 编译工具链路径指定
│    		├── build.sh     ## vendor 编译入口
│    		├── IoTOSconfig     ## vendor 配置主入口
│    		├── makefile        ## vendor 编译 makefile 主入口
│    		├── platform.mk     ## vendor 传给应用的编译参数位置
│    		└── tuyaos_kernel.config ## TKL 的能力支持配置
│
└── tuya.json

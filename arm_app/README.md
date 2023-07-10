# POSIX标准环境的SDK工程说明

## 快速使用

### 工具安装

- embed

```shell
curl -fsSL https://registry.code.tuya-inc.top/embed_ci_space_group/embed_install/raw/master/get-embed.sh | sh
embed cache clean --all
embed repo init
embed install embed
```

### 常用命令

- 手动下载依赖组件和工具链，并生成编译配置文件

```shell
./prepare.sh
make sdk
```

### 其它命令

- 常用 make 目标

| 名称   | 说明         |
| ------ | ----------- |
| clean  | 清除中间文件 |
| config | 生成 tuya_iot_config.h |
| sdk    | 编译SDK库 |
| pack   | 打包SDK，包含开发环境 |
| demo   | 编译DEMO程序 |
| sdk_demo | 在SDK产物包环境中，编译DEMO程序 |
| ut     | 编译单元测试 |
| ut_run | 运行单元测试 |

- CI系统调用的构建命令

CI系统传给ci_autobuild.sh的参数：

| 参数 | 示例 | 说明 |
| ---- | ---- | ---- |
| $1 | tuya_iot_wifi_tls | SDK项目名称 |
| $2 | 4.1.3 | SDK产物版本 |
| $3 | linux-ubuntu-6.2.0_64Bit | 开发环境名称 |
| $4 | output/dist/tuya_iot_wifi_tls_linux-ubuntu-6.2.0_64Bit_4.1.3.tar.gz | SDK产物包文件位置，CI系统从这个位置拿到产物包并上传 |

## CI流程

### 构建/正式构建

在 CI 系统的 WEB 页面点击构建或正式构建后，启动构建流程，如下：

1. CI后台将SDK的相关信息传递给Jenkins服务器
2. Jenkins 将SDK依赖的开发环境和组件信息写入 make.yaml 文件
3. Jenkins 将配置内容写入 build.conf 文件
4. 如果是正式构建，将版本描述写入 CHANGELOG.md 文件
5. 提交修改的文件到GIT仓库
6. 打上版本TAG

### 编译/生成产物

在 CI 系统的 WEB 页面点击编译或生成产物后，启动编译流程，如下：

1. CI后台将SDK的相关信息传递给Jenkins服务器
2. Jenkins 调用 ci_autobuild.sh 开始编译
3. 如果点击的是生成产物，则将编译生成的产物包上传到 [maven 服务器](https://maven.tuya-inc.top/nexus/content/repositories/releases/embed/)

## 最小文件集

以下是SDK工程至少要包含的文件：

| 文件名 | 描述 |
| ----- | ---- |
| make.yaml       | 依赖的开发环境和组件的描述文件 |
| ci_autobuild.sh | 提供给CI系统调用的编译脚本 |

## SDK编译流程

ci_autobuild.sh 的处理流程：

1. 根据 make.yaml 拉取组件代码和开发环境文件
2. 根据 build.conf 生成 build_param 文件
3. 根据 Makefile 编译组件，生成SDK产物包
4. 将SDK产物包存放在指定位置，供CI系统上传发布

## 产物包文件结构标准

SDK产物包用于品类工程，在品类工程中解压安装。为了方便品类工程的管理，SDK产物包内的文件按照一定的标准进行组织。

SDK产物包内文件的树形结构：

```text
tuya_iot_wifi_tls_ubuntu_0.1.0.tar.gz
└── tuya_iot_wifi_tls_ubuntu_0.1.0
    ├── demos
    ├── sdk
    │   ├── component1
    │   │   └── include
    │   ├── component2
    │   │   └── include
    │   ├── lib
    │   │   └── libtuya_iot.a
    │   └── CHANGELOG.md
    ├── platforms
    │   └── ubuntu
    └── build_app.sh
```

产物包内的 build_app.sh 传入的参数

| 参数 | 示例 | 说明 |
| ---- | ---- | ---- |
| $1 | demo/hello | app 代码相对路径 |
| $2 | hello      | app 项目名称 |
| $3 | 1.0.0      | app 版本 |

## 配置相关

配置相关的脚本见下表，完成了从 build.conf 生成 build_param 的任务：

| 文件名 | 描述 |
| ----- | ---- |
| build.conf       | 配置定义文件，由CI系统写入 |
| build/prepare_param.sh | 生成 build_param 文件 |
| build/build_param.template | build_param 模板  |

## 单元测试

编译所有单元测试

```shell
make ut
```

编译单个单元测试，如编译svc_http的单元测试:

```shell
make ut TEST=svc_http
```

编译并运行所有单元测试

```shell
make ut_run
```

编译并运行单个单元测试，如编译并运行svc_http的单元测试:

```shell
make ut_run TEST=svc_http
```

## 脚本工具集

### xmake 框架

Makefile 可以自由定义，只要能完成CI系统的任务即可。也可以采用 xmake 编译框架，配合[使用 xmake 的组件模板](https://registry.code.tuya-inc.top/ci_embed_example_group/ci_test_template_component/blob/master/README.md)，代入到 Makefile 中。详见[xmake 编译框架说明](https://code.registry.wgine.com/embed_ci_space_group/xmake/blob/master/README.md)。

- 更新xmake

```shell
./scripts/xmake_update.sh
```

### 通用脚本

更新通用脚本

```
./scripts/tools_update.sh
```

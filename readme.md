# 1. 将本工程放置与 common 同一个父目录下
# 2. 配置环境参数
```
    source common/env_arm.sh
```
# 3. 编译 arm_app
```
    cd arm_app
    mkdir build & cd build
    cmaker
    make
```

# 4. 协议
tuya app 和算法模块通过 lcm message 交互, lcm message 在 common/message 中
lcm message 定义为一个结构体,其使用方法类似 ros message
tuya app 所需要用到的 message 如下
```
message/app_map.lcm                     // App 展示地图
message/app_path.lcm                    // App 展示轨迹
message/robot_status.lcm                // 机器状态
message/event.lcm                       // 一些消息,消息定义见 common/robot_msg.h
message/app_partition_division.lcm      // 分区消息
message/app_partition_merge.lcm         // 合并分区
message/app_restricted_area.lcm         // 禁区
message/app_room_clean.lcm              // 房间清扫
message/app_set_clean_sequence.lcm      // 设定清扫顺序
message/app_set_room_name.lcm           // 设定房间名字
message/app_set_room_properties.lcm     // 设定房间清扫属性
message/app_spot_clean.lcm              // 定点清扫
message/app_virtual_wall.lcm            // 虚拟墙
message/app_zone_clean.lcm              // 划区清扫
message/app_local_alert.lcm             // 本地定时
message/app_not_disturb_time.lcm        // 勿扰模式
message/app_parts_life.lcm              // 组件寿命
message/app_clean_info.lcm              // 清扫信息
message/app_clean_record.lcm            // 清扫记录
```
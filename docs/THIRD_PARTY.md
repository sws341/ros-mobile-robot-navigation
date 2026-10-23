# 第三方组件与上游来源

本项目的个人贡献是机械设计、系统集成、ROS 配置和下位机适配，不把第三方算法或驱动声明为原创。

| 组件 | 使用方式 | 上游与许可证说明 |
|---|---|---|
| ROSArduinoBridge / `ros_arduino_bridge` | 仓库保留修改后的 Arduino 固件；Python ROS 端未复制 | [hbrobotics/ros_arduino_bridge](https://github.com/hbrobotics/ros_arduino_bridge)，历史 `indigo-devel` 分支；固件保留 BSD 声明，Python ROS 包按其上游许可 |
| `rplidar_ros` | 外部依赖 | [Slamtec/rplidar_ros](https://github.com/Slamtec/rplidar_ros)，上游仓库标注 BSD-2-Clause；应选择与设备和 ROS 1 环境兼容的版本 |
| `usb_cam` | 外部依赖 | [ros-drivers/usb_cam](https://github.com/ros-drivers/usb_cam)，上游为 BSD 许可；当前默认分支面向 ROS 2，历史 ROS 1 工程应选择匹配分支/版本 |
| GMapping、AMCL、Move Base、Map Server | ROS 1 外部依赖 | 使用开源 ROS 导航栈；本仓库只保存 Launch/YAML 集成配置 |
| RViz、Xacro、TF、robot/joint state publisher | ROS 1 外部依赖 | 运行与可视化依赖，未复制源码 |

原始归档中包含第三方 Git 仓库和 catkin 编译产物；公开版已剔除这些整仓副本、`.git`、`build/` 和 `devel/`。安装依赖时请检查目标提交或发行版自己的许可证，不能用本仓库的 MIT 文件覆盖上游条款。

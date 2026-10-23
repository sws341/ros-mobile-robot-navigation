# 个人贡献与原创性边界

## 可归属于本项目作者的工作

- 履带底盘、三级剪叉升降机构、四自由度机械臂和夹爪的机械方案与样机集成。
- Arduino UNO R3、Sensor Shield V5、L298N、四路编码器、雷达和相机的硬件集成与历史调试。
- `car_start`、`daohang`、`mycar_description` 中针对本项目的 Launch、YAML、地图和 URDF/Xacro 配置。
- 基于 ROSArduinoBridge 的四轮编码器、L298N PWM、PID 和串口协议适配。
- 本仓库的整理、图源索引、风险审查和静态验证脚本。

## 不应声明为原创的内容

- GMapping、AMCL、Move Base、ROSArduinoBridge、`rplidar_ros`、`usb_cam` 及其他 ROS 基础组件。
- catkin 自动生成模板或上游项目中未由作者独立编写的框架代码。
- 任何没有当前构建/实验记录支持的性能数字或“开箱即用”结论。

## 平台与实验边界

履带式机器人是机械设计样机；论文中的实景 ROS 导航与四目标点演示来自独立四轮实验平台。相机承担监控/显示用途，本项目不宣称完成相机-LiDAR 融合导航。

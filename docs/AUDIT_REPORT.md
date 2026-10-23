# 源码整理审查报告

## 原始材料概况

- 原代码 ZIP 共 1,674 个条目（含目录），混有 catkin `build/`、`devel/`、第三方依赖和上游 `.git/`。
- 项目专用 ROS 包为 `car_start`、`daohang`、`mycar_description`；`ros_arduino_bridge`、`rplidar_ros`、`usb_cam` 为第三方项目。
- 原始材料存在独立顶层与工作空间内两份 `ros_arduino_bridge`。作者确认采用独立顶层 `ROSArduinoBridge4wd`；论文附录也更接近该版本。历史烧录二进制和上传日志未保存，因此不能把源码一致性扩展为烧录证据。

## 公开版清理

1. 只保留 3 个项目 ROS 包、两组地图和经确认的 Arduino 固件；剔除编译缓存、第三方整仓复制和嵌套 `.git`。
2. 将地图 YAML 的历史用户目录绝对路径改为同目录相对路径，地图像素和数值参数未改。
3. 将串口参数集中到 `car_start/config/arduino_params.yaml`，不再要求修改第三方包内配置。
4. 保留 ROSArduinoBridge 的 BSD 版权，并用 `NOTICE` 区分 MIT、自有图片和第三方内容。
5. 当前保留论文中与本人项目直接相关的 10 张实物、架构、接线和实验图。
6. 移除发布前临时脚本、重复静态日志和会误导读者的过期状态清单。

## 源码审查结果

- V0.4 已修复 D13 LED/电机方向复用和 `READ_ENCODERS` 多余回复。
- v1.0.0 修复 PID 小数被 `int` 截断、在线调参整数解析、串口参数缓冲越界风险和两个 ROS include 路径错误。
- 三个 ROS 包补齐维护者、版本、许可证、仓库 URL 和运行依赖。
- 仍未解决的运行风险包括 Python 2 时代 `ros_arduino_python` 兼容性、底盘尺寸/编码器参数实测、方向极性、TF 冲突和硬件急停。

## 验证边界

`scripts/validate_repo.py` 对仓库结构、XML/YAML、本地路径、README 链接、图片签名、敏感文件、占位符和关键固件规则执行静态验收。当前环境没有 ROS 1 和 Arduino 工具链，未做 catkin/UNO 编译、串口联调或实机复测。详情见 [`VALIDATION.md`](VALIDATION.md)。
